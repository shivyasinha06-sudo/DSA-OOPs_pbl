# IntelliPlag

A plagiarism detection system built for a 3rd-semester college PBL, combining  
**Data Structures & Algorithms**, **OOP in C++**, and a **246 000-article SQLite corpus**.

---

## Architecture

```
Browser (frontend/index.html)
        │  POST /api/plagiarism/check
        ▼
Flask API  (api/server.py)          ← Python, no shell=True
        │  subprocess.run([main, --json, tmpfile, db])
        ▼
C++ Engine  (main.cpp)              ← compiled as main / main.exe
        │  opens
        ▼
SQLite  (data/plagiarism.db)        ← 198 MB, tracked via Git LFS
        │  returns 2 000 articles
        ▼
DSA Pipeline
  Preprocessor → NGram (4-gram) → HashTable → InvertedIndex
  → Trie → KMP StringMatcher → MaxHeap → ContainmentSimilarity / JaccardSimilarity
        │
        ▼
JSON result → API → frontend
```

---

## DSA & OOP Concepts Demonstrated

| Concept | Where |
|---|---|
| Abstract base class + polymorphism | `CorpusLoaderBase`, `SimilarityMetric`, `PatternMatcher` |
| Template class | `Vector<T>` |
| Hash table (chaining) | `HashTable.h`, `InvertedIndex.h` |
| Prefix Trie | `Trie.h` |
| Max-Heap + operator overloading | `MaxHeap.h`, `Result` class |
| KMP string matching | `StringMatcher.h` |
| N-gram fingerprinting | `NGram.h` |
| SQLite integration | `SQLiteDB.h`, `CorpusLoader.h` |

---

## Local Setup

### 1 — Compile the C++ engine (Windows)

```powershell
gcc -c sqlite3.c -o sqlite3.o
g++ -I. main.cpp sqlite3.o -o main.exe
```

### 1 — Compile the C++ engine (Linux / macOS)

```bash
gcc -c sqlite3.c -o sqlite3.o
g++ -I. main.cpp sqlite3.o -o main
```

### 2 — Install Python dependencies

```bash
pip install -r api/requirements.txt
```

### 3 — Start the web server

```bash
python api/server.py
```

Open `http://localhost:5000` in your browser.

---

## Command-line usage (C++ engine only)

```bash
# Plain-text report
./main input.txt data/plagiarism.db

# JSON output (used by the API)
./main --json input.txt data/plagiarism.db
```

---

## API

### `POST /api/plagiarism/check`

**Request**
```json
{ "title": "optional title", "text": "document text to check" }
```

**Response**
```json
{
  "overall_verdict": "PLAGIARISED | SUSPICIOUS | CLEAN",
  "status": "MATCHES_FOUND | CLEAN",
  "articlesIndexed": 2000,
  "uniqueNGrams": 21,
  "nGramSize": 4,
  "matches": [
    {
      "rank": 1,
      "title": "April",
      "verdict": "PLAGIARISED",
      "containment": 42.86,
      "jaccard": 2.21,
      "matchedPhrase": "is the fourth month",
      "charOffset": 6
    }
  ]
}
```

### `GET /health`

Returns engine and database availability.

---

## Database

`data/plagiarism.db` (~198 MB) is tracked with **Git LFS**.  
The raw source file `AllCombined.txt` is excluded from the repository.

To pull the database after cloning:
```bash
git lfs pull
```

---

## Deployment

The frontend is served by the same Flask process — no separate static host is needed.  
GitHub Pages **cannot** be used because it is static-only and cannot execute the C++ engine.

**Recommended free deployment options:**

| Platform | Notes |
|---|---|
| [Render.com](https://render.com) | Free tier, Python web service, supports compiled binaries |
| [Railway.app](https://railway.app) | Free tier, easy GitHub integration |
| Linux VPS (any) | Full control; see deploy.yml SSH option |

**Start command (all platforms):**
```
python api/server.py
```

**Build command (Linux server):**
```
gcc -c sqlite3.c -o sqlite3.o && g++ -I. main.cpp sqlite3.o -o main
```

---

## Repository

Branch: `IntelliPlag`  
Remote: `https://github.com/shivyasinha06-sudo/DSA-OOPs_pbl.git`
