#include <iostream>
#include <vector>
using namespace std;

enum Type {
  BOOL = 1999,
  INT,
  FLOAT,
  CHAR,
  STRING,
  CONST,
  ID,

  ASTERISK, // *
  SLASH,   // /
  PERCENT, // %
  PLUS,   // +
  MINUS, // -

  ASSIGN, // =
  AND, // &&
  OR, // ||
  NOT, // !

  EQUAL, // ==
  NOT_EQUAL, // !=
  GT,       // >
  LT,       // <
  GE,      // >=
  LE,    // <=

  LP,  // (
  RP, // )
  LB, // [
  RB, // ]
  LC, // {
  RC, // }

  PLUS_ASSIGN, // +=
  MINUS_ASSIGN, // -=
  MULT_ASSIGN, // *=
  DIV_ASSIGN, // /=
  MOD_ASSIGN, // %=

  PP, // ++
  MM, // --
  RS, // >>
  LS, // <<

  COMMA, // ,
  SEMICOLON, // ;
  Question, // ?
  Colon, // :

  IF, // if
  ELSE, // else
  DO, // do
  WHILE, // while
  RETURN, // return

  COUT, // cout
  NONE, // none
  DONE // done

}; // Type

struct Token {
  Type type;
  string value;
}; // Token

struct IDs {
  string name;
  Type type;
  string idName;
  int index;
  vector<string> value;
}; // ID

Token gtoken, gpeek;

void SkipSpace();
Token TokenInit();
Token PeekToken();
Token GetToken();
void User_input();
bool Definition();
bool Type_specifier();
bool Function_definition_or_declarators();
bool Rest_of_declarators();
bool Compound_statement();
bool Declaration();
bool Statement();
bool Expression();
bool Basic_expression();
bool Rest_of_Identifier_started_basic_exp();
bool Rest_of_Cout(); // grammar 表上面沒有的 function
bool Rest_of_PPMM_Identifier_started_basic_exp();
bool Sign();
bool Assignment_operator();
bool Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp();
bool Rest_of_maybe_logical_OR_exp();
bool Maybe_logical_AND_exp();
bool Rest_of_maybe_logical_AND_exp();
bool Maybe_bit_OR_exp();
bool Rest_of_maybe_bit_OR_exp();
bool Maybe_bit_ex_OR_exp();
bool Rest_of_maybe_bit_ex_OR_exp();
bool Maybe_bit_AND_exp();
bool Rest_of_maybe_bit_AND_exp();
bool Maybe_equality_exp();
bool Rest_of_maybe_equality_exp();
bool Maybe_relational_exp();
bool Rest_of_maybe_relational_exp();
bool Maybe_shift_exp();
bool Rest_of_maybe_shift_exp();
bool Maybe_additive_exp();
bool Rest_of_maybe_additive_exp();
bool Maybe_mult_exp();
bool Rest_of_maybe_mult_exp();
bool Unary_exp();
bool Signed_unary_exp();
bool Unsigned_unary_exp();
void CheckIDifExist( string name );

vector<vector<IDs>> gID;
struct Node {
  string name;
  Type type;
  vector<Node*> child;
}; // Node

/**
 * @brief 以遞迴概念遍歷及查看文法樹結構 用來 debug
 * @param node 當前節點
 * @param prefix 前綴字串 (用來表示節點的深度)
 * @return void
 */
void PrintTree(const Node* node, const string& prefix = "", bool isLast = true) {
  if (node == nullptr) return;
  cout << prefix;

  if (isLast) {
    cout << "`-- ";  // 使用 '`-- ' 來表示末節點
  } else {
    cout << "|-- ";  // 使用 '|-- ' 來表示非末節點
  }

  cout << node->name << endl;

  for (size_t i = 0; i < node->child.size(); ++i) {
    PrintTree(node->child[i], prefix + (isLast ? "    " : "|   "), i == node->child.size() - 1);
  }
}

/**
 * @brief 以遞迴概念遍歷及刪除文法樹結構 用來釋放記憶體以避免 memory leak
 * @param node 當前節點
 * @return void
 */
void DeleteTree(Node* node) {
  if (node == nullptr) return; // 基本情況：節點為空

  // 遞迴刪除所有子節點
  for (Node* child : node->child) {
    DeleteTree(child);
  }

  // 刪除當前節點
  delete node;
}


Node * gReturnNode = nullptr;
int gLine;

/**
 * @brief 自製STOI函數，將string轉換為int
 * @param str 要轉換的字串
 * @return num 轉換後的數字
 */
int STOI(string str) {
  int num = 0;
  bool isNegative = false;
  int startIndex = 0;

  if (str[0] == '-') {
    isNegative = true;
    startIndex = 1;
  }

  for (int i = startIndex; i < str.size(); i++) {
    num = num * 10 + (str[i] - '0');
  } // for

  if (isNegative) {
    num = -num;
  }

  return num;
} // STOI()

/**
 * @brief 自製STOF函數，將string轉換為float
 * @param str 要轉換的字串
 * @return num 轉換後的數字
 */
float STOF(string str) {
  float num = 0;
  bool isNegative = false;
  int startIndex = 0;

  if (str[0] == '-') {
    isNegative = true;
    startIndex = 1;
  }

  int i = startIndex;
  for (; i < str.size(); i++) {
    if (str[i] == '.') {
      i++;
      break;
    } // if

    num = num * 10 + (str[i] - '0');
  } // for

  float dec = 0.1;
  for (; i < str.size(); i++) {
    num += (str[i] - '0') * dec;
    dec /= 10;
  } // for

  if (isNegative) {
    num = -num;
  }

  return num;
} // STOF()

bool gNegative, gIsCout;
IDs GetIDvalue( string name, int index );
void SetIDvalue( string name, int index, IDs id );
void Cal( Node *node );
void CalDefinition( Node *node );
IDs IDsCal( IDs a, IDs b, string op );
IDs CalOther( Node *node );

/**
 * @brief 控制主程式流程 開始 -> 過程 -> 結束
 */
int main() {
  int testnum = 0;
  cin >> testnum;
  cout << "Our-C running ..." << endl;
  gtoken = gpeek = TokenInit();
  gID.push_back( vector<IDs>() );
  while ( true ) {
    while ( gID.size() > 1 ) { // 只留下main function那一層的ID
      gID.pop_back();
    } // while

    try {
      cout << "> ";
      gLine = 0; // 因為每次input的起始點都是上一行的"\n"，所以每次開始都要歸零
      gID.push_back( vector<IDs>() ); // 新增一個scope(避免感染到main function的ID)

      User_input();
      // PrintTree( gReturnNode );
      if ( gReturnNode->child.size() != 0 && gReturnNode->child[0]->child.size() != 0 
            && gReturnNode->child[0]->child[0]->name == "Done" ) { // Done() 結束程式
        break;
      } // if

      while ( gID.size() > 1 ) { // 只留下main function那一層的ID
        gID.pop_back();
      } // while

      // cout << "Cal .." << endl;
      Cal( gReturnNode );
    } // try

    catch (const string &msg) {
      cout << "Line " << gLine << " : " << msg << endl;
      while ( cin.peek() != '\n' && cin.peek() != -1 ) { // 跳過錯誤的那一行
        cin.get();
      } // while

      gpeek = gtoken = TokenInit();
    } // catch
    
    //DeleteTree( gReturnNode );

  } // while

  cout << "Our-C exited ..." << endl;
} // main()

void SkipSpace() {
  while (cin.peek() == '\n' || cin.peek() == ' ' || cin.peek() == '\t') {
    if (cin.peek() == '\n') {
      gLine++;
    } // if
    cin.get();
  } // while
} // SkipSpace()

Token TokenInit() {
  Token token;
  token.type = NONE;
  return token;
} // TokenInit()

/**
 * @brief 需要的時候會偷看下一個 token，連續偷看不會一直往下看除非使用過GetToken()
 * @return Token 回傳下一個(還未被Get的token)
 */
Token PeekToken() {
  if (gpeek.type != NONE) {
    return gpeek;
  } // if

  SkipSpace();

  Token token = TokenInit();
  char first = cin.get();

  token.value += first;

  if (first == '\0') { // EOF
    token.type = DONE;
    token.value = "done";
    cout << "hi" << endl;
  } // if

  else if (first == '+') { // + ++ +=
    token.type = PLUS;
    if (cin.peek() == '+') {
      token.type = PP;
      token.value += cin.get();
    } // if
    else if (cin.peek() == '=') {
      token.type = PLUS_ASSIGN;
      token.value += cin.get();
    } // else if

  } // else if

  else if (first == '-') { // - -- -=
    token.type = MINUS;
    if (cin.peek() == '-') {
      token.type = MM;
      token.value += cin.get();
    } // if
    else if (cin.peek() == '=') {
      token.type = MINUS_ASSIGN;
      token.value += cin.get();
    } // else if
  } // else if

  else if (first == '*') { // * *=
    token.type = ASTERISK;
    if (cin.peek() == '=') {
      token.type = MULT_ASSIGN;
      token.value += cin.get();
    } // if
  } // else if

  else if (first == '/') { // / /=
    token.type = SLASH;
    if (cin.peek() == '=') {
      token.type = DIV_ASSIGN;
      token.value += cin.get();
    } // if

    else if ( cin.peek() == '/' ) {
      while ( cin.peek() != '\n' ) {
        token.value += cin.get();
      } // while
      token = PeekToken();
    } // else if

  } // else if

  else if (first == '%') { // % %=
    token.type = PERCENT;
    if (cin.peek() == '=') {
      token.type = MOD_ASSIGN;
      token.value += cin.get();
    } // if
  } // else if

  else if (first == '=') { // = ==
    token.type = ASSIGN;
    if (cin.peek() == '=') {
      token.type = EQUAL;
      token.value += cin.get();
    } // if
  } // else if

  else if (first == '&') { // & &&
    token.type = AND;
    if (cin.peek() == '&') {
      token.type = AND;
      token.value += cin.get();
    } // if
  } // else if

  else if (first == '|') { // | ||
    token.type = OR;
    if (cin.peek() == '|') {
      token.type = OR;
      token.value += cin.get();
    } // if
  } // else if

  else if (first == '!') { // ! !=
    token.type = NOT;
    if (cin.peek() == '=') {
      token.type = NOT_EQUAL;
      token.value += cin.get();
    } // if
  } // else if

  else if (first == '>') { // > >> >=
    token.type = GT;
    if (cin.peek() == '>') {
      token.type = RS;
      token.value += cin.get();
    } // if

    else if (cin.peek() == '=') {
      token.type = GE;
      token.value += cin.get();
    } // else if
  } // else if

  else if (first == '<') { // < << <=
    token.type = LT;
    if (cin.peek() == '<') {
      token.type = LS;
      token.value += cin.get();
    } // if

    else if (cin.peek() == '=') {
      token.type = LE;
      token.value += cin.get();
    } // else if
  } // else if

  else if (first == '(') {
    token.type = LP;
  } // else if

  else if (first == ')') {
    token.type = RP;
  } // else if

  else if (first == '[') {
    token.type = LB;
  } // else if

  else if (first == ']') {
    token.type = RB;
  } // else if

  else if (first == '{') {
    token.type = LC;
  } // else if

  else if (first == '}') {
    token.type = RC;
  } // else if

  else if (first == ',') {
    token.type = COMMA;
  } // else if

  else if (first == ';') {
    token.type = SEMICOLON;
  } // else if

  else if (first == '?') {
    token.type = Question;
  } // else if

  else if (first == ':') {
    token.type = Colon;
  } // else if

  else if (first == '\'') { // char
    token.type = CONST;
    token.value += cin.get();
    token.value += cin.get();
  } // if

  else if (first == '\"') { // string
    char pre = first;
    token.type = CONST;
    while (cin.peek() != '\"') {
      if (pre == '\\' && cin.peek() == 'n') {
        token.value[token.value.size() - 1] = '\n';
        cin.get();
        pre = '\n';
      } // if

      else if ( cin.peek() == '@' && token.value.length() >= 4 && token.value[token.value.size() - 1] == token.value[token.value.size() - 2] 
                && token.value[token.value.size() - 2] == token.value[token.value.size() - 3] && token.value[token.value.size() - 3] == token.value[token.value.size() - 4]
                && token.value[token.value.size() - 1] == '@' ) { // 為了解決換行字元變成"@@@@@"的問題
        token.value.erase( token.value.end() - 4, token.value.end() );
        token.value += '\n' ;
        cin.get();
        pre = '\n';
      } // else if

      else
        token.value  += cin.get();
      pre = token.value[token.value.size() - 1];
    } // while
    token.value += cin.get();
  } // else if

  else if (isdigit(first)) { // int or float
    token.type = CONST;
    while (cin.peek() >= '0' && cin.peek() <= '9') {
      token.value += cin.get();
    } // while

    if (cin.peek() == '.') {
      token.value += cin.get();
      while (cin.peek() >= '0' && cin.peek() <= '9') {
        token.value += cin.get();
      } // while
    } // if
  } // else if

  else if (first == '.') {
    token.type = CONST;
    while (cin.peek() >= '0' && cin.peek() <= '9') {
      token.value += cin.get();
    } // while
  } // else if

  else if (isalpha(first) || first == '_') {
    token.type = ID;
    while (cin.peek() != -1 && (isalpha(cin.peek()) || cin.peek() == '_'
                                || isdigit(cin.peek()))) {
      token.value += cin.get();
    } // while

    if (token.value == "true" || token.value == "false") {
      token.type = CONST;
    } // if

    else if (token.value == "int") {
      token.type = INT;
    } // else if

    else if (token.value == "float") {
      token.type = FLOAT;
    } // else if

    else if (token.value == "char") {
      token.type = CHAR;
    } // else if

    else if (token.value == "string") {
      token.type = STRING;
    } // else if

    else if (token.value == "bool") {
      token.type = BOOL;
    } // else if

    else if (token.value == "if") {
      token.type = IF;
    } // else if

    else if (token.value == "else") {
      token.type = ELSE;
    } // else if

    else if (token.value == "while") {
      token.type = WHILE;
    } // else if

    else if ( token.value == "do" ) {
      token.type = DO;
    } // else if

    else if ( token.value == "return" ) {
      token.type = RETURN;
    } // else if

    else if ( token.value == "cout" ) {
      token.type = COUT;
    } // else if

    else if ( token.value == "Done" ) {
      token.type = DONE;
    } // else if

  } // else if
  
  else {
    throw "unrecognized token with first char '" + token.value + "'";
  } // else

  gpeek = token;
  return gpeek;
} // PeekToken()

/**
 * @brief 取得 token 並將 gpeek 初始化以便下次 PeekToken
 * @return Token 回傳已取得的 token
 */
Token GetToken() {
  Token token = PeekToken();
  gpeek = TokenInit();
  gtoken = token;
  // cout << "now token: " << token.value << " type: " << token.type << endl;
  return gtoken;
} // GetToken()

// ------------------- 以下為文法規則在Parseing 的過程中順便建立文法樹 -------------------
void User_input() {
  PeekToken();
  if ( Definition() ) {
    Node *node = new Node();
    node->name = "Definition";
    node->child.push_back( gReturnNode );
    gReturnNode = node;
  } // if

  else if ( Statement() ) {
    Node *node = new Node();
    node->name = "Statement";
    if ( gReturnNode != nullptr ) {
      node->child.push_back( gReturnNode );
    } // if

    gReturnNode = node;
  } // else if

  else {
    throw "unexpected token '" + gpeek.value + "'";
  } // else
} // User_input()

bool Definition() {
  if ( Type_specifier() ) {
    PeekToken();
    if ( gpeek.type == ID ) {
      gReturnNode -> child.push_back( new Node() );
      gReturnNode->child[0]->type = gpeek.type;
      gReturnNode->child[0]->name = gpeek.value;
      GetToken();
      if ( Function_definition_or_declarators() ) {
        return true;
      } // if
    } // if
  } // else if

  return false;
} // Definition()

bool Type_specifier() {
  PeekToken();
  if ( gpeek.type == INT || gpeek.type == CHAR || gpeek.type == FLOAT
        || gpeek.type == STRING || gpeek.type == BOOL ) {
    gReturnNode = new Node();
    gReturnNode->name = "Type_specifier";
    gReturnNode->type = gpeek.type;
    GetToken();
    return true;
  } // if

  return false;
} // Type_specifier()


bool Function_definition_or_declarators() {
  if ( Rest_of_declarators() ) {
    return true;
  } // else if

  return false;
} // Function_definition_or_declarators()

bool Rest_of_declarators() {
  IDs id;
  id.name = gReturnNode->child[0]->name;
  gID[gID.size() - 1].push_back( id );
  PeekToken();
  if ( gpeek.type == LB ) {
    GetToken();
    PeekToken();
    if ( gpeek.type == CONST ) {
      gReturnNode->child[0]->child.push_back( new Node() );
      gReturnNode->child[0]->child[0]->type = gpeek.type;
      gReturnNode->child[0]->child[0]->name = gpeek.value;
      GetToken();
      PeekToken();
      if ( gpeek.type == RB ) {
        GetToken();
      } // if
    } // if
  } // if

  PeekToken();
  while ( gpeek.type == COMMA ) {
    GetToken();
    PeekToken();
    if ( gpeek.type == ID ) {
      id.name = gpeek.value;
      gID[gID.size() - 1].push_back( id );
      gReturnNode->child.push_back( new Node() );
      gReturnNode->child[gReturnNode->child.size() - 1]->type = gpeek.type;
      gReturnNode->child[gReturnNode->child.size() - 1]->name = gpeek.value;
      GetToken();
      PeekToken();
      if ( gpeek.type == LB ) {
        GetToken();
        PeekToken();
        if ( gpeek.type == CONST ) {
          gReturnNode->child[gReturnNode->child.size() - 1]->child.push_back( new Node() );
          gReturnNode->child[gReturnNode->child.size() - 1]->child[0]->type = gpeek.type;
          gReturnNode->child[gReturnNode->child.size() - 1]->child[0]->name = gpeek.value;
          GetToken();
          PeekToken();
          if ( gpeek.type == RB ) {
            GetToken();
          } // if
        } // if
      } // if
    } // if
    PeekToken();
  } // while

  PeekToken();
  if ( gpeek.type == SEMICOLON ) {
    GetToken();
    return true;
  } // if

  return false; // error exit
} // Rest_of_declarators()
// here
bool Compound_statement() {
  PeekToken();
  Node *node = new Node();
  node->name = "Compound_statement";
  if ( gpeek.type == LC ) {
    gID.push_back( vector<IDs>() ); // 進入新的scope
    GetToken();
    while ( Declaration() || Statement() ) {
      if ( gReturnNode != nullptr ) {
        node->child.push_back( gReturnNode );
      } // if
    } // while

    PeekToken();
    if ( gpeek.type == RC ) {
      gID.pop_back(); // 離開scope
      gReturnNode = node;
      GetToken();
      return true;
    } // if
  } // if

  return false;
} // Compound_statement()

bool Declaration() {
  if ( Type_specifier() ) {
    PeekToken();
    if ( gpeek.type == ID ) {
      gReturnNode -> child.push_back( new Node() );
      gReturnNode->child[0]->type = gpeek.type;
      gReturnNode->child[0]->name = gpeek.value;
      GetToken();
      if ( Rest_of_declarators() ) {
        return true;
      } // if
    } // if
  } // if

  return false;
} // Declaration()

bool Statement() {
  PeekToken();
  if ( gpeek.type == SEMICOLON ) {
    gReturnNode = nullptr;
    GetToken();
    return true;
  } // if

  else if ( Expression() ) {
    PeekToken();
    if ( gpeek.type == SEMICOLON ) {
      GetToken();
      return true;
    } // if
  } // else if

  else if ( Compound_statement() ) {
    return true;
  } // else if

  else if ( gpeek.type == IF ) {
    Node *node = new Node();
    node->name = "If";
    GetToken();
    PeekToken();
    if ( gpeek.type == LP ) {
      GetToken();
      if ( Expression() ) {
        node->child.push_back( gReturnNode );
        PeekToken();
        if ( gpeek.type == RP ) {
          GetToken();
          if ( Statement() ) {
            node->child.push_back( gReturnNode );
            PeekToken();
            if ( gpeek.type == ELSE ) {
              GetToken();
              if ( Statement() ) {
                node->child.push_back( gReturnNode );
                gReturnNode = node;
                return true;
              } // if
            } // if

            else {
              gReturnNode = node;
              return true;
            } // else
          } // if
        } // if
      } // if
    } // if
  } // else if

  else if ( gpeek.type == WHILE ) {
    Node *node = new Node();
    node->name = "While";
    GetToken();
    PeekToken();
    if ( gpeek.type == LP ) {
      GetToken();
      if ( Expression() ) {
        node->child.push_back( gReturnNode );
        PeekToken();
        if ( gpeek.type == RP ) {
          GetToken();
          if ( Statement() ) {
            node->child.push_back( gReturnNode );
            gReturnNode = node; 
            return true;
          } // if
        } // if
      } // if
    } // if
  } // else if

  else if ( gpeek.type == DO ) {
    Node *node = new Node();
    node->name = "Do";
    GetToken();
    if ( Statement() ) {
      node->child.push_back( gReturnNode );
      PeekToken();
      if ( gpeek.type == WHILE ) {
        GetToken();
        PeekToken();
        if ( gpeek.type == LP ) {
          GetToken();
          if ( Expression() ) {
            node->child.push_back( gReturnNode );
            PeekToken();
            if ( gpeek.type == RP ) {
              GetToken();
              PeekToken();
              if ( gpeek.type == SEMICOLON ) {
                gReturnNode = node;
                GetToken();
                return true;
              } // if
            } // if
          } // if
        } // if
      } // if
    } // if
  } // else if

  return false;
} // Statement()

bool Expression() {
  Node *node = new Node();
  node->name = "Expression";
  if ( Basic_expression() ) {
    node->child.push_back( gReturnNode );
    PeekToken();
    while ( gpeek.type == COMMA ) {
      GetToken();
      if ( Basic_expression() ) {
        node->child.push_back( gReturnNode );
        PeekToken();
      } // if
    } // while

    gReturnNode = node;
    return true;
  } // if

  return false;
} // Expression()

bool Basic_expression() {
  PeekToken();
  gNegative = false;
  if ( gpeek.type == ID ) {
    gReturnNode = new Node();
    gReturnNode->name = gpeek.value;
    gReturnNode->type = gpeek.type;
    GetToken();
    CheckIDifExist( gReturnNode->name );
    if ( Rest_of_Identifier_started_basic_exp() ) {
      return true;
    } // if
  } // if

  else if ( gpeek.type == COUT ) {
    gReturnNode = new Node();
    gReturnNode->name = "cout";
    gReturnNode->type = gpeek.type;
    GetToken();
    bool preIsCout = gIsCout;
    gIsCout = true;
    Rest_of_Cout();
    gIsCout = preIsCout;
    return true;
  } // else if

  else if ( gpeek.type == DONE ) {
    gReturnNode = new Node();
    gReturnNode->name = "Done";
    gReturnNode->type = gpeek.type;
    GetToken();

    PeekToken();
    while ( gpeek.type != SEMICOLON ) {
      GetToken();
      PeekToken();
    } // while

    return true;
  } // else if

  else if ( gpeek.type == PP || gpeek.type == MM ) {
    Node *node = new Node();
    node->name = "PPMMFront";
    node->type = gpeek.type;
    GetToken();
    PeekToken();
    if ( gpeek.type == ID ) {
      gReturnNode = new Node();
      gReturnNode->name = gpeek.value;
      gReturnNode->type = gpeek.type;
      node -> child.push_back( gReturnNode );
      gReturnNode = node;
      GetToken();
      CheckIDifExist( gReturnNode->child[0]->name );
      if ( Rest_of_PPMM_Identifier_started_basic_exp() ) {
        return true;
      } // if
    } // if
  } // else if

  else if ( Sign() ) {
    while ( Sign() ) {
      
    } // while

    if ( Signed_unary_exp() ) {
      if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
        // 需要判斷是否為負數
        return true;
      } // if
    } // if
  } // else if

  else if ( gpeek.type == CONST ) {
    gReturnNode = new Node();
    gReturnNode->name = gpeek.value;
    gReturnNode->type = gpeek.type;
    GetToken();
    if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
      return true;
    } // if
  } // else if

  else if ( gpeek.type == LP ) {
    bool preIsCout = gIsCout;
    gIsCout = false;
    GetToken();
    if ( Expression() ) {
      PeekToken();
      if ( gpeek.type == RP ) {
        gIsCout = preIsCout;
        GetToken();
        if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
          return true;
        } // if
      } // if
    } // if
  } // else if

  return false;
} // Basic_expression()

bool Rest_of_Identifier_started_basic_exp() {
  PeekToken();
  Node *id = gReturnNode;
  if ( gpeek.type == LB ) {
    GetToken();
    if ( Expression() ) {
      PeekToken();
      if ( gpeek.type == RB ) {
        GetToken();
        id -> child.push_back( gReturnNode );
        gReturnNode = id;
      } // if
    } // if

  } // if

  if ( Assignment_operator() ) {
    Node *assign = gReturnNode;
    assign -> child.push_back( id );
    if ( Basic_expression() ) {
      assign -> child.push_back( gReturnNode );
      gReturnNode = assign;
      return true;
    } // if
  } // if

  else if ( gpeek.type == PP || gpeek.type == MM ) {
    Node *node = new Node();
    node->name = "PPMMBack";
    node->type = gpeek.type;
    GetToken();
    node -> child.push_back( id );
    gReturnNode = node;
    if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
      
    } // if

    return true;
  } // else if

  else if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
    return true;
  } // else if

  return false;

} // Rest_of_Identifier_started_basic_exp()

bool Rest_of_Cout() {
  PeekToken();
  Node* coutNode = gReturnNode;
  while ( gpeek.type == LS ) {
    GetToken();
    if ( Basic_expression() ) {
      coutNode->child.push_back( gReturnNode );
      PeekToken();
    } // if

  } // while

  gReturnNode = coutNode;
  return true;
} // Rest_of_Cout()

bool Rest_of_PPMM_Identifier_started_basic_exp() {
  Node *id = gReturnNode->child[0];
  Node *node = gReturnNode;
  PeekToken();
  if ( gpeek.type == LB ) {
    GetToken();
    if ( Expression() ) {
      PeekToken();
      if ( gpeek.type == RB ) {
        GetToken();
        id -> child.push_back( gReturnNode );
        gReturnNode = node;
        if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
          return true;
        } // if
      } // if
    } // if
  } // if

  else {
    if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
      return true;
    } // if
  } // else

  return false;
} // Rest_of_PPMM_Identifier_started_basic_exp()

bool Sign() {
  PeekToken();
  if ( gpeek.type == PLUS || gpeek.type == MINUS || gpeek.type == NOT ) {
    if ( gpeek.type == MINUS || gpeek.type == NOT ) {
      gNegative = !gNegative;
    } // if
    GetToken();
    return true;
  } // if

  return false;
} // Sign()

bool Assignment_operator() {
  PeekToken();
  if ( gpeek.type == ASSIGN || gpeek.type == PLUS_ASSIGN || gpeek.type == MINUS_ASSIGN
       || gpeek.type == MULT_ASSIGN || gpeek.type == DIV_ASSIGN || gpeek.type == MOD_ASSIGN ) {
    gReturnNode = new Node();
    gReturnNode->name = gpeek.value;
    gReturnNode->type = gpeek.type;
    GetToken();
    return true;
  } // if

  return false;
} // Assignment_operator()

bool Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() {
  if ( Rest_of_maybe_logical_OR_exp() ) {
    PeekToken();
    while ( gpeek.type == Question ) {
      Node *ques = new Node();
      ques->name = "Question";
      ques->type = gpeek.type;
      ques->child.push_back( gReturnNode );
      GetToken();
      if ( Basic_expression() ) {
        ques->child.push_back( gReturnNode );
        PeekToken();
        if ( gpeek.type == Colon ) {
          GetToken();
          if ( Basic_expression() ) {
            ques->child.push_back( gReturnNode );
            gReturnNode = ques;
            return true;
          } // if
        } // if
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp()

bool Rest_of_maybe_logical_OR_exp() {
  if ( Rest_of_maybe_logical_AND_exp() ) {
    PeekToken();
    while ( gpeek.type == OR ) {
      Node *orNode = new Node();
      orNode->name = "||";
      orNode->type = gpeek.type;
      orNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_logical_AND_exp() ) {
        orNode->child.push_back( gReturnNode );
        gReturnNode = orNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_logical_OR_exp()

bool Maybe_logical_AND_exp() {
  if ( Maybe_bit_OR_exp() ) {
    PeekToken();
    while ( gpeek.value == "&&" ) {
      Node *andNode = new Node();
      andNode->name = "&&";
      andNode->type = gpeek.type;
      andNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_bit_OR_exp() ) {
        andNode->child.push_back( gReturnNode );
        gReturnNode = andNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_logical_AND_exp()

bool Rest_of_maybe_logical_AND_exp() {
  if ( Rest_of_maybe_bit_OR_exp() ) {
    PeekToken();
    while ( gpeek.value == "&&" ) {
      Node *andNode = new Node();
      andNode->name = "&&";
      andNode->type = gpeek.type;
      andNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_bit_OR_exp() ) {
        andNode->child.push_back( gReturnNode );
        gReturnNode = andNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_logical_AND_exp()

bool Maybe_bit_OR_exp() {
  if ( Maybe_bit_ex_OR_exp() ) {
    PeekToken();
    while ( gpeek.value == "|" ) {
      Node *orNode = new Node();
      orNode->name = "OR";
      orNode->type = gpeek.type;
      orNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_bit_ex_OR_exp() ) {
        orNode->child.push_back( gReturnNode );
        gReturnNode = orNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_bit_OR_exp()

bool Rest_of_maybe_bit_OR_exp() {
  if ( Rest_of_maybe_bit_ex_OR_exp() ) {
    PeekToken();
    while ( gpeek.value == "|" ) {
      Node *orNode = new Node();
      orNode->name = "OR";
      orNode->type = gpeek.type;
      orNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_bit_ex_OR_exp() ) {
        orNode->child.push_back( gReturnNode );
        gReturnNode = orNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_bit_OR_exp()

bool Maybe_bit_ex_OR_exp() {
  if ( Maybe_bit_AND_exp() ) {
    PeekToken();
    while ( gpeek.type == '^' ) {
      Node *orNode = new Node();
      orNode->name = "XOR";
      orNode->type = gpeek.type;
      orNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_bit_AND_exp() ) {
        orNode->child.push_back( gReturnNode );
        gReturnNode = orNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_bit_ex_OR_exp()

bool Rest_of_maybe_bit_ex_OR_exp() {
  if ( Rest_of_maybe_bit_AND_exp() ) {
    PeekToken();
    while ( gpeek.type == '^' ) {
      Node *orNode = new Node();
      orNode->name = "XOR";
      orNode->type = gpeek.type;
      orNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_bit_AND_exp() ) {
        orNode->child.push_back( gReturnNode );
        gReturnNode = orNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_bit_ex_OR_exp()

bool Maybe_bit_AND_exp() {
  if ( Maybe_equality_exp() ) {
    PeekToken();
    while ( gpeek.value == "&" ) {
      Node *andNode = new Node();
      andNode->name = "&";
      andNode->type = gpeek.type;
      andNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_equality_exp() ) {
        andNode->child.push_back( gReturnNode );
        gReturnNode = andNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_bit_AND_exp()

bool Rest_of_maybe_bit_AND_exp() {
  if ( Rest_of_maybe_equality_exp() ) {
    PeekToken();
    while ( gpeek.value == "&" ) {
      Node *andNode = new Node();
      andNode->name = "&";
      andNode->type = gpeek.type;
      andNode->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_equality_exp() ) {
        andNode->child.push_back( gReturnNode );
        gReturnNode = andNode;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_bit_AND_exp()

bool Maybe_equality_exp() {
  if ( Maybe_relational_exp() ) {
    PeekToken();
    while ( gpeek.type == EQUAL || gpeek.type == NOT_EQUAL ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_relational_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_equality_exp()

bool Rest_of_maybe_equality_exp() {
  if ( Rest_of_maybe_relational_exp() ) {
    PeekToken();
    while ( gpeek.type == EQUAL || gpeek.type == NOT_EQUAL ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_relational_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_equality_exp()

bool Maybe_relational_exp() {
  if ( Maybe_shift_exp() ) {
    PeekToken();
    while ( gpeek.type == LT || gpeek.type == GT || gpeek.type == LE || gpeek.type == GE ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_shift_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_relational_exp()

bool Rest_of_maybe_relational_exp() {
  if ( Rest_of_maybe_shift_exp() ) {
    PeekToken();
    while ( gpeek.type == LT || gpeek.type == GT || gpeek.type == LE || gpeek.type == GE ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_shift_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_relational_exp()

bool Maybe_shift_exp() {
  if ( Maybe_additive_exp() ) {
    PeekToken();
    if ( gpeek.type == LS && gIsCout ) {
      return true;
    } // if

    while ( gpeek.type == LS || gpeek.type == RS ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_additive_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_shift_exp()

bool Rest_of_maybe_shift_exp() {
  if ( Rest_of_maybe_additive_exp() ) {
    PeekToken();
    if ( gpeek.type == LS && gIsCout ) {
      return true;
    } // if
    
    while ( gpeek.type == LS || gpeek.type == RS ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_additive_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_shift_exp()

bool Maybe_additive_exp() {
  if ( Maybe_mult_exp() ) {
    PeekToken();
    while ( gpeek.type == PLUS || gpeek.type == MINUS ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_mult_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Maybe_additive_exp()

bool Rest_of_maybe_additive_exp() {
  if ( Rest_of_maybe_mult_exp() ) {
    PeekToken();
    while ( gpeek.type == PLUS || gpeek.type == MINUS ) {
      Node *node = new Node();
      node->name = gpeek.value;
      node->type = gpeek.type;
      node->child.push_back( gReturnNode );
      GetToken();
      if ( Maybe_mult_exp() ) {
        node->child.push_back( gReturnNode );
        gReturnNode = node;
      } // if

      PeekToken();
    } // while

    return true;
  } // if

  return false;
} // Rest_of_maybe_additive_exp()

bool Maybe_mult_exp() {
  if ( Unary_exp() ) {
    if ( Rest_of_maybe_mult_exp() ) {
      return true;
    } // if
  } // if

  return false;
} // Maybe_mult_exp()

bool Rest_of_maybe_mult_exp() {
  PeekToken();
  while ( gpeek.type == ASTERISK || gpeek.type == SLASH || gpeek.type == PERCENT ) {
    Node *node = new Node();
    node->name = gpeek.value;
    node->type = gpeek.type;
    node->child.push_back( gReturnNode );
    GetToken();
    if ( Unary_exp() ) {
      node->child.push_back( gReturnNode );
      gReturnNode = node;
    } // if

    PeekToken();
  } // while

  return true;
} // Rest_of_maybe_mult_exp()

bool Unary_exp() {
  PeekToken();
  gNegative = false;
  if ( Sign() ) {
    while ( Sign() ) {

    } // while

    if ( Signed_unary_exp() ) {
      return true;
    } // if
  } // if

  else if ( Unsigned_unary_exp() ) {
    return true;
  } // else if

  else if ( gpeek.type == PP || gpeek.type == MM ) {
    Node *node = new Node();
    node->name = "PPMMFront";
    node->type = gpeek.type;
    GetToken();
    PeekToken();
    if ( gpeek.type == ID ) {
      Node *id = new Node();
      id->name = gpeek.value;
      id->type = gpeek.type;
      node -> child.push_back( id );
      GetToken();
      PeekToken();
      CheckIDifExist( id->name );
      if ( gpeek.type == LB ) {
        GetToken();
        if ( Expression() ) {
          id -> child.push_back( gReturnNode );
          PeekToken();
          if ( gpeek.type == RB ) {
            gReturnNode = node;
            GetToken();
            return true;
          } // if
        } // if
      } // if

      gReturnNode = node;
      return true;
    } // if
  } // else if

  else {
    throw "unexpected token '" + gpeek.value + "'";
  } // else

  return false;
} // Unary_exp()

bool Signed_unary_exp() {
  PeekToken();
  Node *zero = new Node();
  zero->name = "0";
  zero->type = CONST;
  Node *minus = new Node();
  minus->name = "-";
  minus->type = MINUS;
  minus->child.push_back( zero );
  bool isNegative = gNegative;
  if ( gpeek.type == ID ) {
    Node *node = new Node();
    node->name = gpeek.value;
    node->type = gpeek.type;
    GetToken();
    PeekToken();
    CheckIDifExist( node->name );
    if ( gpeek.type == LB ) {
      GetToken();
      if ( Expression() ) {
        node -> child.push_back( gReturnNode );
        PeekToken();
        if ( gpeek.type == RB ) {
          
          if ( isNegative ) {
            minus->child.push_back( node );
            gReturnNode = minus;
          } // if

          else {
            gReturnNode = node;
          } // else

          GetToken();
          return true;
        } // if
      } // if
    } // else if

    if ( isNegative ) {
      minus->child.push_back( node );
      gReturnNode = minus;
    } // if

    else {
      gReturnNode = node;
    } // else
    return true;
  } // if

  else if ( gpeek.type == CONST ) {
    gReturnNode = new Node();
    gReturnNode->name = gpeek.value;
    gReturnNode->type = gpeek.type;
    if ( isNegative ) {
      minus->child.push_back( gReturnNode );
      gReturnNode = minus;
    } // if
    GetToken();
    return true;
  } // else if

  else if ( gpeek.type == LP ) {
    GetToken();
    if ( Expression() ) {
      PeekToken();
      if ( gpeek.type == RP ) {
        if ( isNegative ) {
          minus->child.push_back( gReturnNode );
          gReturnNode = minus;
        } // if
        GetToken();
        return true;
      } // if
    } // if
  } // else if

  return false;
} // Signed_unary_exp()

bool Unsigned_unary_exp() {
  PeekToken();
  if ( gpeek.type == ID ) {
    Node *id = new Node();
    id->name = gpeek.value;
    id->type = gpeek.type;
    GetToken();
    PeekToken();
    CheckIDifExist( id->name );
    if ( gpeek.type == LB ) {
      GetToken();
      PeekToken();
      if ( Expression() ) {
        PeekToken();
        if ( gpeek.type == RB ) {
          GetToken();
          PeekToken();
          id -> child.push_back( gReturnNode );
          if ( gpeek.type == PP || gpeek.type == MM ) {
            Node *node = new Node();
            node->name = "PPMMBack";
            node->type = gpeek.type;
            node -> child.push_back( id );
            gReturnNode = node;
            GetToken();
            return true;
          } // if
        } // if
      } // if
    } // if

    else if ( gpeek.type == PP || gpeek.type == MM ) {
      Node *node = new Node();
      node->name = "PPMMBack";
      node->type = gpeek.type;
      node -> child.push_back( id );
      gReturnNode = node;
      GetToken();
      return true;
    } // if

    
    gReturnNode = id;
    return true;

  } // if

  else if ( gpeek.type == CONST ) {
    gReturnNode = new Node();
    gReturnNode->name = gpeek.value;
    gReturnNode->type = gpeek.type;
    GetToken();
    return true;
  } // else if

  else if ( gpeek.type == LP ) {
    GetToken();
    if ( Expression() ) {
      PeekToken();
      if ( gpeek.type == RP ) {
        GetToken();
        return true;
      } // if
    } // if
  } // else if

  return false;
} // Unsigned_unary_exp()

void CheckIDifExist( string name ) {
  for ( int i = gID.size()-1; i >= 0; i-- ) {
    for ( int j = 0; j < gID[i].size(); j++ ) {
      if ( gID[i][j].name == name ) {
        return;
      } // if
    } // for
  } // for

  throw "undefined identifier '" + name + "'";
} // CheckIDifExist()

// -----------------------------------------------------------------------------------------------

/**
 * @brief 第一層的運算
 * @param node 文法樹的根節點
 * @return void
 */
void Cal( Node *node ) { 
  if ( node->name == "Definition" ) {
    CalDefinition( node->child[0] );
  } // if

  else {
    // cout << CalOther( node->child[0] ).name << endl;
    if ( node->child.size() != 0 ) {
      CalOther( node->child[0] );
    } // if
    cout << "Statement executed ..." << endl;
  } // else

} // Cal()

/**
 * @brief 定義變數(第二層)
 * @param node 定義變數的根節點
 * @return void
 */
void CalDefinition( Node *node ) {
  for( int i = 0; i < node->child.size(); i++ ) {
    IDs id;
    id.name = node->child[i]->name;
    id.type = node->type;
    id.value.resize( 1000 );
    gID[gID.size()-1].push_back( id );
    cout << "Definition of " << node->child[i]->name << " entered ..." << endl;
  } // for

} // CalDefinition()

/**
 * @brief 以遞迴方式完成其他運算(第二層)
 * @param node 其他運算的根節點
 * @return void
 */
IDs CalOther( Node *node ) {
  IDs id;
  if ( node == nullptr ) {
    return id;
  } // if

  
  if ( node->name == "Expression" ) { 
    for ( int i = 0; i < node->child.size(); i++ ) {
      id = CalOther( node->child[i] );
    } // for
  } // if

  else if ( node->name == "Type_specifier" ) { // ex. int a, b, c;
    for( int i = 0; i < node->child.size(); i++ ) {
      id.name = node->child[i]->name;
      id.type = node->type;
      id.value.resize( 1000 );
      gID[gID.size()-1].push_back( id );
    } // for
  } // if

  else if ( node->name == "cout" ) { // ex. cout << a << b << c;
    IDs buff;
    for ( int i = 0; i < node->child.size(); i++ ) {
      buff = CalOther( node->child[i] );
      cout << buff.name;
      id.name += buff.name;
      id.type = buff.type;
    } // for
  } // else if

  else if ( node->type == CONST ) { // 分類出屬於哪一種 CONST 類似解析(一開始輸入的時候沒有特別區分type)
    if( node->name[0] == '\'' ) {
      node->name.erase( 0, 1 );
      node->name.pop_back();
      id.type = CHAR;
    } // if
    else if( node->name[0] == '\"' ) {
      node->name.erase( 0, 1 );
      node->name.pop_back();
      id.type = STRING;
    } // else if
    else if( node->name == "true" || node->name == "false" ) {
      id.type = BOOL;
    } // else if
    else {
      if( node->name.find( '.' ) != string::npos ) {
        id.type = FLOAT;
      } // if
      else {
        id.type = INT;
      } // else
    } // else

    id.name = node->name;
  } // else if
  
  else if ( node->type == ID ) {
    int index = 0;
    if ( node -> child.size() != 0 ) {
      index = STOI( CalOther( node->child[0] ).name );
    } // if

    id = GetIDvalue( node->name, index );
  } // else if

  else if ( node->name == "Compound_statement" ) { // ex. { a = 1; b = 2; c = 3; }
    gID.push_back( vector<IDs>() );
    for ( int i = 0; i < node->child.size(); i++ ) {
      id = CalOther( node->child[i] );
    } // for

    gID.pop_back();

  } // else if

  else if ( node->name == "PPMMFront" ) {
    int index = 0;
    if ( node -> child[0]->child.size() != 0 ) {
      index = STOI( CalOther( node->child[0]->child[0] ).name );
    } // if

    id = GetIDvalue( node->child[0]->name, index );
    if ( node->type == PP ) {
      if ( id.type == FLOAT ) {
        id.name = to_string( STOF( id.name ) + 1 );
      } // if

      else {
        id.name = to_string( STOI( id.name ) + 1 );
      } // else
    } // if

    else {
      if ( id.type == FLOAT ) {
        id.name = to_string( STOF( id.name ) - 1 );
      } // if

      else {
        id.name = to_string( STOI( id.name ) - 1 );
      } // else
    } // else

    SetIDvalue( node->child[0]->name, index, id );
  } // else if

  else if ( node->name == "PPMMBack" ) {
    int index = 0;
    if ( node -> child[0]->child.size() != 0 ) {
      index = STOI( CalOther( node->child[0]->child[0] ).name );
    } // if

    id = GetIDvalue( node->child[0]->name, index );
    if ( node->type == PP ) {
      if ( id.type == FLOAT ) {
        id.name = to_string( STOF( id.name ) + 1 );
      } // if

      else {
        id.name = to_string( STOI( id.name ) + 1 );
      } // else
    } // if

    else {
      if ( id.type == FLOAT ) {
        id.name = to_string( STOF( id.name ) - 1 );
      } // if

      else {
        id.name = to_string( STOI( id.name ) - 1 );
      } // else
    } // else

    SetIDvalue( node->child[0]->name, index, id );

    if ( node->type == PP ) {
      if ( id.type == FLOAT ) {
        id.name = to_string( STOF( id.name ) - 1 );
      } // if

      else {
        id.name = to_string( STOI( id.name ) - 1 );
      } // else
    } // if

    else {
      if ( id.type == FLOAT ) {
        id.name = to_string( STOF( id.name ) + 1 );
      } // if

      else {
        id.name = to_string( STOI( id.name ) + 1 );
      } // else
    } // else
  } // else if

  else if ( node->name == "Question" ) { // ex. a = ( b == 1 ) ? 2 : 3;
    id = CalOther( node->child[0] );
    if ( id.name == "true" ) {
      id = CalOther( node->child[1] );
    } // if

    else {
      id = CalOther( node->child[2] );
    } // else
  } // else if

  else if ( node->name == "If" ) { // ex. if( a == 1 ) { b = 2; } else { b = 3; }
    id = CalOther( node->child[0] );
    if ( id.name == "true" ) {
      id = CalOther( node->child[1] );
    } // if

    else if ( node->child.size() == 3 ) {
      id = CalOther( node->child[2] );
    } // else
  } // else if

  else if ( node->name == "While" ) { // ex. while( a == 1 ) { b = 2; }
    id = CalOther( node->child[0] );
    while ( id.name == "true" ) {
      id = CalOther( node->child[1] );
      id = CalOther( node->child[0] );
    } // while
  } // else if


  else { // basic運算 ex. b + c; b - c; b * c; b / c; b % c; b >> c; b << c; b && c; b || c; b == c; b != c; b < c; b <= c; b > c; b >= c;
    IDs a, b;
    a = CalOther( node->child[0] );
    b = CalOther( node->child[1] );
    id = IDsCal( a, b, node->name );
  } // else

  return id;
} // CalOther()

/**
 * @brief 依照運算子做運算
 * @param a 運算元a
 * @param b 運算元b
 * @param op 運算子
 */
IDs IDsCal( IDs a, IDs b, string op ) {
  IDs result;
  if ( a.type == STRING || b.type == STRING ) {
    result.type = STRING;
  } // if

  else if ( a.type == CHAR || b.type == CHAR ) {
    result.type = CHAR;
  } // else if

  else if ( a.type == BOOL || b.type == BOOL ) {
    result.type = BOOL;
  } // else if

  else if ( a.type == FLOAT || b.type == FLOAT ) {
    result.type = FLOAT;
  } // else if

  else {
    result.type = INT;
  } // else

  if ( op == "+" ) {
    if ( result.type == STRING ) {
      result.name = a.name + b.name;
    } // if

    else if ( result.type == FLOAT ) {
      result.name = to_string( STOF( a.name ) + STOF( b.name ) );
    } // else if

    else {
      result.name = to_string( STOI( a.name ) + STOI( b.name ) );
    } // else
  } // if

  else if ( op == "-" ) {
    if ( result.type == FLOAT ) {
      result.name = to_string( STOF( a.name ) - STOF( b.name ) );
    } // if

    else if ( result.type == BOOL ) {
      result.name = ( b.name == "true" ) ? "false" : "true";
    } // else if

    else {
      result.name = to_string( STOI( a.name ) - STOI( b.name ) );
    } // else
  } // else if

  else if ( op == "*" ) {
    if ( result.type == FLOAT ) {
      result.name = to_string( STOF( a.name ) * STOF( b.name ) );
    } // if

    else {
      result.name = to_string( STOI( a.name ) * STOI( b.name ) );
    } // else
  } // else if

  else if ( op == "/" ) {
    if ( result.type == FLOAT ) {
      result.name = to_string( STOF( a.name ) / STOF( b.name ) );
    } // if

    else {
      result.name = to_string( STOI( a.name ) / STOI( b.name ) );
    } // else
  } // else if

  else if ( op == "%" ) {
    result.name = to_string( STOI( a.name ) % STOI( b.name ) );
  } // else if

  else if ( op == ">>" ) {
    result.name = to_string( STOI( a.name ) >> STOI( b.name ) );
  } // else if

  else if ( op == "<<" ) {
    result.name = to_string( STOI( a.name ) << STOI( b.name ) );
  } // else if

  else if ( op == "&&" ) {
    result.name = ( a.name == "true" && b.name == "true" ) ? "true" : "false";
    result.type = BOOL;
  } // else if

  else if ( op == "||" ) {
    result.name = ( a.name == "true" || b.name == "true" ) ? "true" : "false";
    result.type = BOOL;
  } // else if

  else if ( op == "==" ) {
    result.name = ( a.name == b.name ) ? "true" : "false";
    result.type = BOOL;
  } // else if

  else if ( op == "!=" ) {
    result.name = ( a.name != b.name ) ? "true" : "false";
    result.type = BOOL;
  } // else if

  else if ( op == "<" ) {
    if ( result.type == FLOAT ) {
      result.name = ( STOF( a.name ) < STOF( b.name ) ) ? "true" : "false";
    } // if

    else {
      result.name = ( STOI( a.name ) < STOI( b.name ) ) ? "true" : "false";
    } // else

    result.type = BOOL;
  } // else if

  else if ( op == "<=" ) {
    if ( result.type == FLOAT ) {
      result.name = ( STOF( a.name ) <= STOF( b.name ) ) ? "true" : "false";
    } // if

    else {
      result.name = ( STOI( a.name ) <= STOI( b.name ) ) ? "true" : "false";
    } // else

    result.type = BOOL;
  } // else if

  else if ( op == ">" ) {
    if ( result.type == FLOAT ) {
      result.name = ( STOF( a.name ) > STOF( b.name ) ) ? "true" : "false";
    } // if

    else {
      result.name = ( STOI( a.name ) > STOI( b.name ) ) ? "true" : "false";
    } // else

    result.type = BOOL;
  } // else if

  else if ( op == ">=" ) {
    if ( result.type == FLOAT ) {
      result.name = ( STOF( a.name ) >= STOF( b.name ) ) ? "true" : "false";
    } // if

    else {
      result.name = ( STOI( a.name ) >= STOI( b.name ) ) ? "true" : "false";
    } // else

    result.type = BOOL;
  } // else if

  else if ( op == "!" ) {
    result.name = ( a.name == "true" ) ? "false" : "true";
  } // else if

  else if ( op == "++" ) {
    result.name = to_string( STOI( a.name ) + 1 );
  } // else if

  else if ( op == "--" ) {
    result.name = to_string( STOI( a.name ) - 1 );
  } // else if
  
  else if ( op == "=" ) { // ex. a = b;
    SetIDvalue( a.idName, a.index, b );
    result.name = b.name;
  } // else if

  else if ( op == "+=" ) { // 拆成兩個運算 ex. a += b; -> a = a + b;
    IDs buff = IDsCal( a, b, "+" );
    IDsCal( a, buff, "=" );
    result = buff;
  } // else if

  else if ( op == "-=" ) { // 拆成兩個運算 ex. a -= b; -> a = a - b;
    IDs buff = IDsCal( a, b, "-" );
    IDsCal( a, buff, "=" );
    result = buff;
  } // else if

  else if ( op == "*=" ) { // 拆成兩個運算 ex. a *= b; -> a = a * b;
    IDs buff = IDsCal( a, b, "*" );
    IDsCal( a, buff, "=" );
    result = buff;
  } // else if

  else if ( op == "/=" ) { // 拆成兩個運算 ex. a /= b; -> a = a / b;
    IDs buff = IDsCal( a, b, "/" );
    IDsCal( a, buff, "=" );
    result = buff;
  } // else if

  else if ( op == "%=" ) { // 拆成兩個運算 ex. a %= b; -> a = a % b;
    IDs buff = IDsCal( a, b, "%" );
    IDsCal( a, buff, "=" );
    result = buff;
  } // else if

  return result;
} // IDsCal()

/**
 * @brief 取得變數的值
 * @param name 變數名稱
 * @param index 變數的索引
 * @return IDs 變數的值
 */
IDs GetIDvalue( string name, int index ) {
  IDs id;
  for ( int i = 0; i < gID.size(); i++ ) {
    for ( int j = 0; j < gID[i].size(); j++ ) {
      if ( gID[i][j].name == name ) {
        id.name = gID[i][j].value[index];
        id.type = gID[i][j].type;
        id.idName = gID[i][j].name;
        id.index = index;
      } // if
    } // for
  } // for

  return id;
} // GetIDvalue()

/**
 * @brief 設定變數的值
 * @param name 變數名稱
 * @param index 變數的索引
 * @param value 變數的值
 * @return void
 */
void SetIDvalue( string name, int index, IDs value ) {
  for ( int i = gID.size()-1; i >= 0; i-- ) {
    for ( int j = gID[i].size()-1; j >=0 ; j-- ) {
      if ( gID[i][j].name == name ) {
        gID[i][j].value[index] = value.name;
        return ;
      } // if
    } // for
  } // for

} // SetIDvalue()

