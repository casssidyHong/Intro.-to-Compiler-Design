# hw2 report

|||
|-:|:-|
|Name|洪巧芸|
|ID|110550143|

## How much time did you spend on this project

> 3 days, and about 3-6 hours per day

## Project overview

### Scanner Part
因為基礎的架構就是沿用上次作業的內容，但因為這次要加上parser所以要在每個需要被回傳的輸入後面加上 return。  
(e.g. ":", ":=", "print" 等後面都須要加上我們想回傳的東西，但Whitespace因為要discard，所以我們就不需要在後面加上return)

判斷是否需要回傳:  
1. Whitespace, Pseudocomment, Comment, Newline, Out of scanner rules' -> 不需要在function裡面加上回傳  
1. 其他 -> return

而我將return分為兩個種類:  
1. 單一字符 -> 回傳符號  
   e.g. "+" return '+', "-" return '-'  
1. 其他 -> 英文字串(多將小寫改為大寫回傳)  
   e.g.  
         ":=" return 'ASSIGN'  
         "and" return 'AND'  
         {integer} return DECIMAL_NUMBER  
         0[0-7]+ return OCTAL_NUM

### Parser Part
主要思路：  
照著ReadMe下去打，題目給的變數名稱舉例叫什麼我就設定成什麼，一步一步打。  
如果在某個function中要用到其他的就在後面補上。

**_DEFINITIONS_**  
宣告我們在scanner中會回傳的東西  
1. %left -> expressions(Because their associativity are left.)  
   e.g. %left '+'  
1. %token -> others  
   e.g. %token ARRAY  

%%

**_SYNTACTIC DEFINITION RULES_**
設定符合parser的規則

### Program Units
----
**_PROGRAM_**
```
program
    : ID ';' //最一開始都是以Identifier;的形式開頭
      declaration_list //可能有0或者多個 constant declaration
      function_list //可能有0或者多個 function declaration跟definition
      //最後都是以compound statement end的形式結尾
      compound_statement
      END
    ;
```
**_FUNCTION_**
```
//FUNCTION DECLARATION
function_del
    : ID '(' formal_arguments_list_ ')' ':' scalar_type ';'
    | ID '(' formal_arguments_list_ ')' ';'
    ; 

function_declaration
    : function_del
    ;

//FUNCTION DEFINITION
function_def
    : ID '(' formal_arguments_list_ ')' ':' scalar_type
    | ID '(' formal_arguments_list_ ')'
    ;

//DEFINITION 會以 compound statement end做結尾
function_definition
    : function_def
      compound_statement
      END
    ;

//合併DECLARATION跟DEFINITION
function 
    : function_declaration
    | function_definition
    ;

//因為可以有0或是多個，所以設定為"∅"或是原本的自己再加上新的function
function_list
    : 
    | function_list function
    ;

//function裡面的內容物
formal_arguments
    : identifier_list ':' type 
    ;

//1或多個formal arguments，多個formal arguments的話彼此之間要用";"隔開
formal_arguments_list
    : formal_arguments
    | formal_arguments_list ';' formal_arguments
    ;

//因為可以有0或是多個，所以第一個位置放"∅"
formal_arguments_list_
    :
    | formal_arguments_list
    ;

//因為identifier_list至少有一個identifier，所以第一個位置放"ID"而非"∅"
identifier_list
    : ID
    | identifier_list ',' ID
    ;
```
### DATA TYPES and DECLARATIONS
----
**_VARIABLE_**
```
//因為把scalar_type和array integer_constant of type合併成type了，所以這裡只需要打type
variable_declaration
    : VAR identifier_list ':' type ';'
    ;
```
**_CONSTANT_**
```
constant_declaration
    : VAR identifier_list ':' literal_constant ';'
    ;

//把variable跟constant合併成declaration
declaration
    : variable_declaration
    | constant_declaration
    ;

declaration_list
    : 
    | declaration_list declaration
    ;

type
    : scalar_type
    | ARRAY integer_constant OF type
    ;

//integer, real, string and boolean
scalar_type
    : INTEGER
    | REAL
    | STRING
    | BOOLEAN
    ;

//non-negative
integer_constant
    : DECIMAL_NUM
    | OCTAL_NUM
    ;

real_constant
    : FLOAT
    | SCIENTIFIC_NOTATION
    ;

string_literal
    : STRING_LITERAL
    ;

//an integer/real literal with or without a negative sign, a string literal, true, or false
literal_constant
    : integer_constant
    | '-' integer_constant
    | real_constant
    | '-' real_constant
    | string_literal
    | T
    | F
    ;
```
### Statements
----
```
statement
    : compound_statement
    | simple_statement
    | conditional_statement
    | while_statement
    | for_statement
    | return_statement
    | function_call
    ;

//可以有0或多個statement
statement_list
    : 
    | statement_list statement
    ;
```
**_Compound_**
```
compound_statement
    : START 
      declaration_list
      statement_list
      END
    ;
```
**_Simple_**
```
simple_statement
    : assignment
    | print_statement
    | read_statement
    ;

assignment
    : variable_reference ASSIGN expression ';'
    ;

print_statement
    : PRINT expression ';'
    ;

read_statement
    : READ variable_reference ';'
    ;

//開頭一定是ID，所以第一個放ID
variable_reference
    : ID
    | ID array_reference
    ;

//0或多個由“[]”包起來的expression
array_reference
    : '[' expression ']'
    | array_reference '[' expression ']'
    ;
```
**_Conditional_**
```
conditional_statement
    : IF expression THEN compound_statement
      ELSE compound_statement
      END IF
    | IF expression THEN compound_statement
      END IF
    ;
```
**_While_**
```
while_statement
    : WHILE expression
      DO compound_statement
      END DO
    ;
```
**_For_**
```
for_statement
    : FOR ID ASSIGN integer_constant TO integer_constant
      DO compound_statement
      END DO
    ;
```
**_Return_**
```
return_statement
    : RETURN expression ';'
    ;
```
**_Function Call_**
```
function_call
    : ID '(' expression_list ')' ';'
    ;
```
### Expressions
----
```
expression
    : literal_constant
    | variable_reference
    | ID '(' expression_list ')'
    | '(' expression ')'
    | arithmetic_expression
    ;

arithmetic_expression
    : negative
    | arithmetic
    | relational
    | logical 
    ;

//可以是0或多個expression
expression_list
    :
    | expression_list_
    ;

//多個expression的話用","隔開
expression_list_
    : expression
    | expression_list_ ',' expression
    ;
```
**_Negative_**
```
//負
negative
    : '-' expression %prec '*'
    ;
```
**_Arithmetic_**
```
//+, -, *, /, mod
arithmetic
    : expression '*' expression
    | expression '/' expression
    | expression '%' expression
    | expression '+' expression
    | expression '-' expression
    ;
```
**_Relational_**
```
//<, <=, <>, >=, >, =
relational
    : expression SMALLER expression
    | expression SMALLER_OR_EQUAL expression
    | expression SMALLER_OR_BIGGER expression
    | expression BIGGER_OR_EQUAL expression
    | expression BIGGER expression
    | expression EQUAL expression
    ;
```
**_Logical_**
```
logical
    : expression AND expression
    | expression OR expression
    | NOT expression
    ;
```

%%

**USER CODE**
已經寫好的了



## What is the hardest you think in this project

> in front of logical "NOT" there won't have expression in front of it

## Feedback to T.A.s

> Not required, but bonus point may be given.
