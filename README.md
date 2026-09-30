# C Language Interpreter

A C-like interpreter implemented in C++ for practicing lexical analysis, recursive-descent parsing, abstract syntax tree construction, expression evaluation, and basic control flow.

> This project implements a **subset of C-like syntax** for educational purposes. It is not a full C compiler or a standards-compliant C interpreter.

## Features

- Lexical analysis / tokenization
- Recursive-descent parsing
- Abstract Syntax Tree (AST) construction
- Variable declaration and scope management
- Supported data types: `int`, `float`, `char`, `string`, `bool`
- Arithmetic operators: `+`, `-`, `*`, `/`, `%`
- Comparison operators: `==`, `!=`, `>`, `<`, `>=`, `<=`
- Logical operators: `&&`, `||`, `!`
- Assignment operators: `=`, `+=`, `-=`, `*=`, `/=`, `%=`
- Increment / decrement: `++`, `--`
- Conditional statements: `if`, `else`
- Loop statements: `while`, `do ... while`
- Basic `cout` output
- Identifier checking and basic error reporting

## Project Structure

```text
C-Language-Interpreter/
├── main.cpp
├── sample_input.txt
├── README.md
└── .gitignore
```

## Build

### Linux / macOS

```bash
g++ -std=c++17 main.cpp -o c_interpreter
```

### Windows with MinGW g++

```bash
g++ -std=c++17 main.cpp -o c_interpreter.exe
```

## Run

The program currently reads an initial integer before starting the interpreter.  
The provided `sample_input.txt` includes this required first line.

### Linux / macOS

```bash
./c_interpreter < sample_input.txt
```

### Windows

```bash
c_interpreter.exe < sample_input.txt
```

## Example Input

```cpp
1
int a;
a = 10;
int b;
b = 3;

cout << "a + b = " << a + b << "\n";

if ( a > b ) {
  cout << "a is greater than b\n";
}

while ( b < 5 ) {
  cout << "b = " << b << "\n";
  b++;
}

Done();
```

## Example Output

```text
Our-C running ...
> Definition of a entered ...
> Statement executed ...
> Definition of b entered ...
> Statement executed ...
> a + b = 13
Statement executed ...
> a is greater than b
Statement executed ...
> b = 3
b = 4
Statement executed ...
> Our-C exited ...
```

## Implementation Overview

### 1. Tokenizer

The interpreter reads the input stream and converts characters into tokens such as identifiers, constants, operators, delimiters, and keywords.

### 2. Parser

A recursive-descent parser processes the token stream according to the supported grammar and checks whether statements and expressions are syntactically valid.

### 3. AST

Parsed expressions and statements are represented with tree nodes, allowing the interpreter to preserve operator structure and execution order.

### 4. Evaluation

The evaluator traverses the parsed tree, manages identifiers and scopes, evaluates expressions, performs assignments, and executes control-flow statements.

## Notes

- `Done();` terminates the interpreter session.
- This repository is intended as a portfolio / coursework project demonstrating C++ parsing, debugging, data structures, and interpreter implementation.
- The original source file was converted from CP950/Big5 comments to UTF-8 so Chinese comments display correctly on GitHub.
