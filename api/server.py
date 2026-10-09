"""
IntelliPlag — Flask API Server
Wraps the compiled C++ plagiarism engine and exposes:

  POST /api/plagiarism/check
       { "title": "...", "text": "..." }
  →    { "status": "...", "articlesIndexed": N, ... }

  GET  /health
       { "status": "ok" }

The C++ engine is invoked via subprocess.run (no shell=True).
User text is written to a secure temp file — never interpolated into
a shell command string.
"""

import os
import sys
import json
import tempfile
import subprocess
import io

from flask import Flask, request, jsonify
from flask import send_from_directory

# ── Paths ────────────────────────────────────────────────────────────────────

# api/server.py lives one level below the project root
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# Compiled C++ engine
ENGINE_EXE = os.path.join(PROJECT_ROOT, "main.exe" if os.name == "nt" else "main")
# SQLite corpus database
DB_PATH = os.path.join(PROJECT_ROOT, "data", "plagiarism.db")

# Static frontend files
FRONTEND_DIR = os.path.join(PROJECT_ROOT, "frontend")

# ── Flask app ────────────────────────────────────────────────────────────────

app = Flask(__name__, static_folder=None)


# ── CORS helper (no extra library needed) ────────────────────────────────────

def _add_cors(response):
    response.headers["Access-Control-Allow-Origin"]  = "*"
    response.headers["Access-Control-Allow-Headers"] = "Content-Type"
    response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS"
    return response


@app.after_request
def after_request(response):
    return _add_cors(response)


# ── Serve the frontend ────────────────────────────────────────────────────────

@app.route("/")
def index():
    return send_from_directory(FRONTEND_DIR, "index.html")


@app.route("/<path:filename>", methods=["GET", "HEAD"])
def static_files(filename):
    # Never intercept API routes — those have their own handlers above.
    if filename.startswith("api/") or filename.startswith("health"):
        from flask import abort
        abort(404)
    return send_from_directory(FRONTEND_DIR, filename)


# ── Health check ─────────────────────────────────────────────────────────────

@app.route("/health", methods=["GET"])
def health():
    engine_ok = os.path.isfile(ENGINE_EXE)
    db_ok     = os.path.isfile(DB_PATH)
    return jsonify({
        "status":    "ok" if (engine_ok and db_ok) else "degraded",
        "engine":    engine_ok,
        "database":  db_ok,
    })


# ── Text extraction from uploaded files ──────────────────────────────────────

MAX_UPLOAD_BYTES = 10 * 1024 * 1024   # 10 MB hard limit

@app.route("/api/extract-text", methods=["POST", "OPTIONS"])
def extract_text():
    """
    Accepts a multipart file upload (.txt / .pdf / .docx).
    Returns { "text": "..." } with the extracted plain text.
    The caller then POSTs that text to /api/plagiarism/check as usual.
    No file is stored permanently — everything is processed in memory.
    """
    if request.method == "OPTIONS":
        return jsonify({}), 200

    if "file" not in request.files:
        return jsonify({"error": "No file uploaded. Use field name 'file'."}), 400

    f = request.files["file"]

    if not f or f.filename == "":
        return jsonify({"error": "Empty filename. Please select a file."}), 400

    filename   = f.filename.lower()
    file_bytes = f.read(MAX_UPLOAD_BYTES + 1)

    if len(file_bytes) > MAX_UPLOAD_BYTES:
        return jsonify({"error": "File exceeds 10 MB limit. Please use a smaller document."}), 413

    if len(file_bytes) == 0:
        return jsonify({"error": "Uploaded file is empty."}), 400

    # ── .txt ─────────────────────────────────────────────────────────────────
    if filename.endswith(".txt"):
        for enc in ("utf-8", "utf-8-sig", "latin-1", "cp1252"):
            try:
                text = file_bytes.decode(enc)
                return jsonify({"text": text, "format": "txt"})
            except (UnicodeDecodeError, ValueError):
                continue
        return jsonify({"error": "Could not decode the text file. Save it as UTF-8 and try again."}), 422

    # ── .pdf ─────────────────────────────────────────────────────────────────
    elif filename.endswith(".pdf"):
        try:
            from pypdf import PdfReader
            reader = PdfReader(io.BytesIO(file_bytes))
            pages  = []
            for page in reader.pages:
                t = page.extract_text()
                if t:
                    pages.append(t)
            text = "\n".join(pages).strip()
            if not text:
                return jsonify({"error": "Could not extract text from this PDF. "
                                         "Scanned/image-only PDFs are not supported. "
                                         "Try copy-pasting the text instead."}), 422
            return jsonify({"text": text, "format": "pdf"})
        except Exception as e:
            return jsonify({"error": "PDF extraction failed: " + str(e)}), 422

    # ── .docx ────────────────────────────────────────────────────────────────
    elif filename.endswith(".docx"):
        try:
            from docx import Document
            doc   = Document(io.BytesIO(file_bytes))
            paras = [p.text for p in doc.paragraphs if p.text.strip()]
            text  = "\n".join(paras).strip()
            if not text:
                return jsonify({"error": "The .docx file contains no readable text paragraphs."}), 422
            return jsonify({"text": text, "format": "docx"})
        except Exception as e:
            return jsonify({"error": "DOCX extraction failed: " + str(e)}), 422

    # ── Unsupported ───────────────────────────────────────────────────────────
    else:
        ext = f.filename.rsplit(".", 1)[-1].upper() if "." in f.filename else "unknown"
        return jsonify({
            "error": f".{ext} files are not supported. "
                      "Please upload a .txt, .pdf, or .docx file, "
                      "or paste the text directly."
        }), 415


# ── Main plagiarism check endpoint ───────────────────────────────────────────

@app.route("/api/plagiarism/check", methods=["POST", "OPTIONS"])
def check_plagiarism():
    if request.method == "OPTIONS":
        return jsonify({}), 200

    # ── Validate request ─────────────────────────────────────────────────────
    data = request.get_json(silent=True)
    if not data:
        return jsonify({"error": "Request body must be JSON."}), 400

    text = data.get("text", "").strip()
    if not text:
        return jsonify({"error": "Field 'text' is required and must not be empty."}), 400

    if len(text) > 500_000:
        return jsonify({"error": "Input text exceeds maximum length (500 000 chars)."}), 400

    # ── Verify engine and DB are present ─────────────────────────────────────
    if not os.path.isfile(ENGINE_EXE):
        return jsonify({"error": "Plagiarism engine not found. Please compile main.exe."}), 503

    if not os.path.isfile(DB_PATH):
        return jsonify({"error": "Corpus database not found at data/plagiarism.db."}), 503

    # ── Write text to a secure temp file ─────────────────────────────────────
    # We NEVER concatenate user text into a shell command string.
    tmp_file = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            suffix=".txt",
            delete=False,
            encoding="utf-8"
        ) as f:
            tmp_file = f.name
            f.write(text)

        # ── Invoke C++ engine safely ──────────────────────────────────────────
        # argv: main.exe --json <tmpfile> <dbpath>
        # shell=False — no shell interpolation of user data.
        result = subprocess.run(
            [ENGINE_EXE, "--json", tmp_file, DB_PATH],
            capture_output=True,
            text=True,
            timeout=120,          # 2-min hard limit
            cwd=PROJECT_ROOT
        )

        if result.returncode != 0:
            stderr_msg = result.stderr.strip() if result.stderr else "Engine returned non-zero exit code."
            return jsonify({"error": "Engine error: " + stderr_msg}), 500

        stdout = result.stdout.strip()
        if not stdout:
            return jsonify({"error": "Engine produced no output."}), 500

        # ── Parse JSON from engine stdout ─────────────────────────────────────
        try:
            engine_output = json.loads(stdout)
        except json.JSONDecodeError as e:
            return jsonify({
                "error": "Could not parse engine output.",
                "detail": str(e),
                "raw": stdout[:500]
            }), 500

        # If the engine itself returned an error field
        if "error" in engine_output:
            return jsonify({"error": engine_output["error"]}), 500

        # ── Build response ────────────────────────────────────────────────────
        matches = engine_output.get("matches", [])
        status  = engine_output.get("status", "UNKNOWN")

        # Determine overall verdict for the UI
        overall_verdict = "CLEAN"
        if status == "MATCHES_FOUND" and matches:
            top_containment = matches[0].get("containment", 0)
            if top_containment >= 25.0:
                overall_verdict = "PLAGIARISED"
            else:
                overall_verdict = "SUSPICIOUS"

        response_data = {
            "overall_verdict":  overall_verdict,
            "status":           status,
            "articlesIndexed":  engine_output.get("articlesIndexed", 0),
            "uniqueNGrams":     engine_output.get("uniqueNGrams", 0),
            "nGramSize":        engine_output.get("nGramSize", 4),
            "matches":          matches,
        }

        return jsonify(response_data), 200

    except subprocess.TimeoutExpired:
        return jsonify({"error": "Plagiarism engine timed out (>120s). Try a shorter document."}), 504

    except Exception as e:
        return jsonify({"error": "Unexpected server error: " + str(e)}), 500

    finally:
        # Always clean up the temp file
        if tmp_file and os.path.isfile(tmp_file):
            try:
                os.unlink(tmp_file)
            except OSError:
                pass


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    import webbrowser

    port  = int(os.environ.get("PORT", 5000))
    debug = os.environ.get("DEBUG", "false").lower() == "true"
    url   = f"http://127.0.0.1:{port}"

    engine_ok = os.path.isfile(ENGINE_EXE)
    db_ok     = os.path.isfile(DB_PATH)

    print()
    print("╔══════════════════════════════════════════════════════════╗")
    print("║              IntelliPlag — Starting Server               ║")
    print("╠══════════════════════════════════════════════════════════╣")
    print(f"║  URL      →  {url:<44}║")
    print(f"║  Engine   →  {'FOUND ✓' if engine_ok else 'MISSING ✗  (run: g++ -I. main.cpp sqlite3.o -o main.exe)':<44}║")
    print(f"║  Database →  {'FOUND ✓' if db_ok else 'MISSING ✗  (data/plagiarism.db not found)':<44}║")
    print("╚══════════════════════════════════════════════════════════╝")
    print()

    if not engine_ok:
        print("[ERROR] main.exe not found. Compile first:")
        print("        gcc -c sqlite3.c -o sqlite3.o")
        print("        g++ -I. main.cpp sqlite3.o -o main.exe")
        print()

    if not db_ok:
        print("[ERROR] data/plagiarism.db not found.")
        print()

    # Open browser after a short delay so Flask has time to bind the port
    if engine_ok and db_ok:
        webbrowser.open(url)

    app.run(host="0.0.0.0", port=port, debug=debug)
