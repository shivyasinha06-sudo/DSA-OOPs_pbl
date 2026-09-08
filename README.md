#dsa-oops_pbl
# IntelliDebug 

### Machine Learning-Based Static Code Analyzer and Debugging Assistant

IntelliDebug is a **C++ static code analysis and debugging assistant** that detects common programming errors and uses **Machine Learning** to predict their severity.

Instead of simply displaying a long list of compiler or analysis warnings, IntelliDebug attempts to identify which issues should receive attention first by classifying them into **Low, Medium, and High severity levels**.

The project combines **Data Structures, Object-Oriented Programming, Compiler Design concepts, Static Analysis, and Machine Learning** into a single practical system.

---

##  Key Features

*  Static analysis of C++ source code
*  Lexical analysis and parsing
*  Abstract Syntax Tree (AST) construction
*  Hash-based symbol table
*  Nested scope management
*  Control Flow Graph (CFG)
*  Rule-based error detection
*  Feature extraction from detected errors
*  Machine Learning-based severity classification
*  Comparison of:

  * Naive Bayes
  * Logistic Regression
  * Support Vector Machine (SVM)
* 📈 Model evaluation using:

  * Accuracy
  * Precision
  * Recall
  * F1-Score
  * Confusion Matrix
*  Selection of the best-performing classifier
*  Priority-based error ranking using a Priority Queue
*  Structured debugging report

---

##  System Workflow

```text
C++ Source Code
       ↓
     Lexer
       ↓
     Parser
       ↓
Abstract Syntax Tree
       ↓
Symbol Table + Scope Management
       ↓
Control Flow Graph
       ↓
Rule-Based Error Engine
       ↓
Feature Extraction
       ↓
┌─────────────────────────────────┐
│ Naive Bayes                     │
│ Logistic Regression             │
│ Support Vector Machine (SVM)    │
└─────────────────────────────────┘
       ↓
Model Evaluation
       ↓
Severity Prediction
(Low / Medium / High)
       ↓
Priority Queue
       ↓
Debugging Report
```

---

##  Technologies Used

### Programming & Development

* **C++**
* **Python**

### Data Structures & Algorithms

* Trees
* Hash Tables
* Stacks
* Graphs
* DFS
* BFS
* Priority Queues

### Static Analysis

* Lexical Analysis
* Parsing
* Abstract Syntax Trees
* Symbol Tables
* Scope Analysis
* Control Flow Graphs
* Rule-Based Analysis

### Machine Learning

* Naive Bayes
* Logistic Regression
* Support Vector Machine (SVM)

### Evaluation

* Accuracy
* Precision
* Recall
* F1-Score
* Confusion Matrix

---

##  Errors Detected

The rule-based analysis engine is designed to identify common programming problems such as:

| Error Type                 | Description                                          |
| -------------------------- | ---------------------------------------------------- |
| Undefined Variable         | Use of a variable that has not been declared         |
| Duplicate Declaration      | Multiple declarations within the same scope          |
| Type Mismatch              | Incompatible data types used in an operation         |
| Invalid Function Arguments | Incorrect number or type of function arguments       |
| Unreachable Code           | Code that cannot be executed                         |
| Unused Variable            | Declared variables that are never used               |
| Suspicious Loops           | Potentially problematic or unintended loop behaviour |

---

##  Machine Learning Approach

Once an error is detected, IntelliDebug extracts relevant features that can help determine its severity.

### Example Features

* Error type
* Scope depth
* AST depth
* Error frequency
* Control-flow impact

These features are provided to multiple classification algorithms.

### Models Compared

**Naive Bayes**
Used as a lightweight probabilistic classification baseline.

**Logistic Regression**
Used to model the relationship between extracted error features and severity classes.

**Support Vector Machine (SVM)**
Used to identify decision boundaries between different severity levels.

The models are evaluated using standard classification metrics, and the best-performing classifier is selected for the final severity prediction.

---

##  Severity Classification

Detected errors are categorized into three levels:

```text
LOW
 │
 ├── Minor impact
 │
MEDIUM
 │
 ├── Requires attention
 │
HIGH
 │
 └── Significant potential impact
```

After classification, a **Priority Queue** organizes errors so that higher-priority issues can be addressed first.

---

##  Project Architecture

```text
                 ┌──────────────────┐
                 │   C++ Source     │
                 │      Code        │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │      Lexer       │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │      Parser      │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │       AST        │
                 └────────┬─────────┘
                          ↓
              ┌───────────┴───────────┐
              ↓                       ↓
       Symbol Table              Scope Management
              │                       │
              └───────────┬───────────┘
                          ↓
                 ┌──────────────────┐
                 │       CFG        │
                 │    DFS + BFS     │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │   Error Engine   │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │ Feature Extraction│
                 └────────┬─────────┘
                          ↓
              ┌───────────┼───────────┐
              ↓           ↓           ↓
          Naive Bayes     LR         SVM
              └───────────┼───────────┘
                          ↓
                 ┌──────────────────┐
                 │ Severity Model   │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │  Priority Queue  │
                 └────────┬─────────┘
                          ↓
                 ┌──────────────────┐
                 │ Debugging Report │
                 └──────────────────┘
```

---

##  Project Structure

```text
IntelliDebug/
│
├── src/
│   ├── lexer/
│   ├── parser/
│   ├── ast/
│   ├── symbol_table/
│   ├── scope/
│   ├── cfg/
│   ├── error_engine/
│   └── main.cpp
│
├── ml/
│   ├── dataset/
│   ├── preprocessing/
│   ├── models/
│   └── evaluation/
│
├── include/
│
├── tests/
│
├── examples/
│   ├── sample1.cpp
│   └── sample2.cpp
│
├── reports/
│
├── README.md
└── requirements.txt
```

*The structure may evolve as development progresses.*

---

##  How It Works

### 1. Source Code Input

The user provides a C++ source file for analysis.

### 2. Lexical Analysis

The lexer breaks the source code into meaningful tokens such as keywords, identifiers, operators, literals, and symbols.

### 3. Parsing & AST Construction

The parser analyzes the tokens and constructs an **Abstract Syntax Tree** representing the structure of the program.

### 4. Symbol & Scope Analysis

A hash-based symbol table stores information about identifiers, variables, and functions. Stack-based scope management handles nested scopes.

### 5. Control Flow Analysis

A Control Flow Graph represents possible execution paths. **DFS and BFS** are used for graph traversal and analysis.

### 6. Error Detection

The rule engine applies predefined rules to identify programming errors.

### 7. Feature Extraction

Relevant characteristics of each detected error are converted into machine-learning features.

### 8. Severity Prediction

Multiple ML classifiers predict whether an error belongs to the Low, Medium, or High severity class.

### 9. Error Prioritization

A priority queue ranks the detected issues according to their predicted severity.

### 10. Debugging Report

The system produces a structured report containing the detected errors, their severity, and their priority.

---

##  Project Objectives

* Develop a functional static analyzer for a selected subset of C++.
* Detect common programming errors using rule-based analysis.
* Apply data structures and algorithms to practical code analysis.
* Extract meaningful features from detected errors.
* Compare multiple machine learning classification algorithms.
* Predict the severity of detected programming errors.
* Prioritize errors using a priority queue.
* Generate a clear and structured debugging report.
* Demonstrate the practical integration of **DSA, OOP, Static Analysis, and ML**.

---

##  Scope

The initial version focuses on a manageable subset of C++, including:

* Variables
* Basic data types
* Expressions
* Functions
* Conditional statements
* Loops
* Basic nested scopes

The system is not intended to replace a complete industrial compiler or static-analysis suite. Instead, it focuses on demonstrating the integration of software analysis techniques with machine learning.

---

##  Future Scope

Potential improvements include:

* Support for a larger subset of C++.
* Detection of additional error categories.
* More sophisticated AST and CFG analysis.
* Larger and more diverse labelled datasets.
* Additional machine learning models.
* Explainable severity predictions.
* Interactive debugging interface.
* IDE integration.
* Real-time code analysis.
* Code-quality and maintainability analysis.

---

##  Team

### IntelliDebug

**Team ID:** `DSCPP-III-2026-T069`

| Member                 | Role        |
| ---------------------- | ----------- |
| Sinha, Shivya          | Team Member |
| Kuletha, Srishti       | Team Member |
| Jagwan, Antriksh Singh | Team Member |

---

##  References

1. Bjarne Stroustrup, *The C++ Programming Language*, Addison-Wesley.
2. A. V. Aho, M. S. Lam, R. Sethi, J. D. Ullman, *Compilers: Principles, Techniques, and Tools*, Pearson.
3. T. H. Cormen, C. E. Leiserson, R. L. Rivest, C. Stein, *Introduction to Algorithms*, MIT Press.
4. Christopher M. Bishop, *Pattern Recognition and Machine Learning*, Springer.
5. Documentation and academic resources related to Naive Bayes, Logistic Regression, and Support Vector Machines.

---

##  Project Summary

**IntelliDebug** aims to bridge the gap between traditional static analysis and intelligent error prioritization by combining compiler concepts, data structures, object-oriented programming, and machine learning.

> **Detect the error. Understand its impact. Fix what matters first.**
