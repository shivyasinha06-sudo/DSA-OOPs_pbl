#  PlagiCheck: Local Plagiarism Detection Engine

An object-oriented, high-performance local plagiarism detector built from scratch using core **Data Structures & Algorithms (DSA)** and **Object-Oriented Programming (OOP)** principles. 

Unlike heavy AI-based tools or external web-crawling services, **PlagiCheck** provides fast, deterministic, and memory-efficient document comparison using custom-built data structures—without relying on external search or database libraries.

---

##  Project Overview

**PlagiCheck** processes text documents through a structured pipeline: cleaning incoming text, chunking content into $n$-grams, indexing sequence fingerprints using custom data structures, and performing high-speed string matching. Matches are evaluated using similarity algorithms and prioritized through a Max-Heap to generate a detailed plagiarism report.

---

##  System Architecture
```
PLAGICHECK PIPELINE
│
▼
Input Documents
│
▼
Text Preprocessing
(lowercase, punctuation)
│
▼
Tokenization
│
▼
┌───────────────────┐
│    DSA MODULES    │
└───────────────────┘
│
┌───────┼───────┐
▼       ▼       ▼
Trie Hash Table Vector
│       │       │
└───────┼───────┘
▼
N-Gram Generation
│
▼
Phrase/String Matching
│
▼
Similarity Calculation
│
▼
Matching Sections Found
│
▼
Similarity Percentage
│
▼
Priority / Ranking
│
▼
Plagiarism Report
```

---

##  Core Features & Custom DSA Modules

Instead of using standard library wrappers, key components are built from scratch to demonstrate low-level algorithmic design:

* **Dynamic Vector:** Stores clean token sequences and preserves exact character offset locations for accurate match highlighting.
* **Custom Hash Table:** Stores $n$-gram fingerprints to enable $O(1)$ constant-time sequence lookup across documents.
* **Prefix Trie:** Indexes sentence prefixes for rapid continuous sequence and exact phrase matching.
* **Max-Heap Priority Queue:** Automatically ranks source document matches by similarity severity so the highest matches appear first.
* **Text Preprocessor:** Handles case normalization, whitespace stripping, and punctuation filtering.

---

##  How It Works

1. **Ingestion & Cleaning:** Documents are loaded, stripped of noise (symbols, punctuation), and converted to lowercase.
2. **$N$-Gram Generation:** Text is divided into overlapping word sequences of length $n$ using a sliding window.
3. **Indexing:** Phrases and fingerprints are inserted into the Trie and Hash Table.
4. **Matching & Scoring:** Overlapping fingerprints are evaluated using the Jaccard Similarity formula.
5. **Reporting:** Results are pushed to a Max-Heap and outputted as a ranked report with similarity percentages and matched offsets.

---

##  Assumptions & Constraints

* **Supported Formats:** Input files must be plain text (`.txt`) encoded in UTF-8.
* **Matching Scope:** Targets exact text copies and structural rearrangements (not deep generative AI paraphrasing).
* **Execution:** All data structures run in-memory inside system RAM for low latency.

---

##  Tech Stack

* **Language:** C++ / Java
* **Design Pattern:** Object-Oriented Architecture (OOP)
* **Core Concepts:** Hash Tables, Trie Trees, Dynamic Arrays, Priority Queues (Max-Heap), N-Grams, String Matching
