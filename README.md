# C Language Interpreter

使用 C++ 實作的簡易 C-like Interpreter，主要練習 Tokenizer、Recursive Descent Parser、AST 建立、變數管理與基本控制流程。

## Features

- Lexical Analysis / Tokenizer
- Recursive Descent Parser
- Abstract Syntax Tree (AST)
- 變數宣告與 Scope 管理
- 基本資料型態
  - `int`
  - `float`
  - `char`
  - `string`
  - `bool`
- 算術運算
  - `+ - * / %`
- 比較運算
  - `== != > < >= <=`
- 邏輯運算
  - `&& || !`
- 指派運算
  - `= += -= *= /= %=`
- `++`、`--`
- `if / else`
- `while`
- `do-while`
- `cout`
- 基本錯誤處理

## Project Structure

```text
C-Language-Interpreter/
├── main.cpp
├── sample_input.txt
├── README.md
└── .gitignore
```

## Run

程式會先讀取一個測試編號，再開始執行 Interpreter。

可以使用 `sample_input.txt` 作為測試輸入。

```bash
./c_interpreter < sample_input.txt
```

Windows：

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

## Implementation Overview

### 1. Tokenizer

將輸入字串解析成不同類型的 Token，例如：

- Identifier
- Constant
- Operator
- Keyword
- Delimiter

並處理像是：

```text
+  -  *  /  %
== != >= <=
&& ||
++ --
```

等運算符號。

### 2. Parser

使用 Recursive Descent Parsing 依照語法規則解析輸入程式。

不同層級的運算會分別處理，例如：

- Assignment
- Logical Expression
- Comparison
- Addition / Subtraction
- Multiplication / Division

以維持正確的運算優先順序。

### 3. Abstract Syntax Tree

解析過程中建立 AST（Abstract Syntax Tree），用來表示程式中的運算式與控制流程。

每個 Node 會保存：

- Node 名稱
- Token Type
- Child Nodes

### 4. Variable & Scope Management

使用資料結構保存變數資訊，並處理不同 Scope 中的變數。

支援：

- 變數宣告
- 變數查找
- 變數賦值
- Array index
- Scope 管理

### 5. Statement Execution

依照 AST 執行不同類型的 Statement，包括：

```cpp
if / else
while
do-while
cout
```

以及一般運算式與變數操作。
