Markdown
# IFJ 2025 Project – Compiler Implementation

[![License: GPL-3.0](https://img.shields.io/badge/License-GPL%203.0-blue.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/Language-C-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

This repository contains a team project for the **Formal Languages and Compilers (IFJ)** course. It is an implementation of a compiler for the selected programming language (`.wren` files). 

The compiler translates source code into the target language (IFJcode) and handles the entire process from lexical analysis to code generation.

## 👥 Authors (Team)

The project was developed collaboratively by:
* **Michal Málik** – [GitHub Profile](https://github.com/michalsmalik) 
* **Matej Mikuš** – [GitHub Profile](https://github.com/matejsk8) 
* **Ján Černoch** – [GitHub Profile](https://github.com/honzulka) 
* **Alexander Klusaček** – [GitHub Profile](https://github.com/ciler22) 

## ⚙️ Compiler Architecture

The compiler is written in **C** and is divided into the following logical components:

1. **Lexical Analyzer (Scanner):** Reads the input source code and converts it into a stream of tokens.
2. **Syntax Analyzer (Parser):** Analyzes the token structure using a top-down approach (recursive descent / LL(1) grammar).
3. **Semantic Analyzer:** Verifies type compatibility, variable declarations, and other semantic rules.
4. **Code Generator:** Generates instructions in the output language based on a valid abstract syntax tree (or linear representation).

## 📂 Repository Structure

* `/src` – Compiler source codes (scanner, parser, etc.).
* `/test` and `/testdir` – Test suites for syntax and semantic analysis.
* `example_files/` – Sample source codes for the compiler.
* `dokumentace.pdf` – Complete project documentation.
* `Makefile` – File for build automation.

## 🚀 Installation & Build (Makefile)

The compiler does not require any complex external libraries; the standard C library and the GCC compiler are sufficient. 

To build the project using the `Makefile`, open your terminal in the root directory of the project and run:

```bash
make
This command will compile all source files in the src/ directory and create an executable binary (usually named compiler or ifj25).

To clean the directory by removing compiled .o files and the binary, run:

Bash
make clean
💻 Usage
The compiler is typically executed by reading the source code from standard input (stdin). Here is an example using one of the test files:

Bash
./main < test_factorial_simple.wren
If the compilation is successful, the program prints the generated code to standard output (stdout) and returns exit code 0. In the event of an error (lexical, syntax, semantic, etc.), it returns the appropriate error code as defined by the IFJ project specification.

🧪 Testing
The repository includes a comprehensive test suite designed to verify the correctness of the syntax and semantic analysis.

To run the tests, you can use the provided bash scripts (e.g., is_it_ok.sh for basic structure validation, and other test scripts inside the test directory):

Bash
cd test/
./run_tests.sh   # (adjust this to the exact name of your test script)
📄 License
This project is licensed under the GPL-3.0 License. For more information, see the LICENSE file.
