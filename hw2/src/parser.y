%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

extern int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

    /* Delimiter */
%token ','
%token ';'
%token ':'
%token '('
%token ')'
%token '['
%token ']'

    /* Operator */
%left '+'
%left '-'
%left '*'
%left '/'
%left '%'  
%token ASSIGN
%left SMALLER
%left SMALLER_OR_EQUAL
%left SMALLER_OR_BIGGER
%left BIGGER_OR_EQUAL
%left BIGGER
%left EQUAL
%left AND
%left OR
%left NOT

    /* Reserved Word */
%token VAR
%token ARRAY
%token OF
%token BOOLEAN
%token INTEGER
%token REAL
%token STRING

%token T
%token F

%token DEF
%token RETURN

%token START
%token END

%token WHILE
%token DO

%token IF
%token THEN
%token ELSE

%token FOR
%token TO

%token PRINT
%token READ

    /*others*/
%token ID
%token DECIMAL_NUM
%token OCTAL_NUM
%token FLOAT
%token SCIENTIFIC_NOTATION
%token STRING_LITERAL

%%

program
    : ID ';' 
      declaration_list
      function_list
      compound_statement 
      END
    ;

function_del
    : ID '(' formal_arguments_list_ ')' ':' scalar_type ';'
    | ID '(' formal_arguments_list_ ')' ';'
    ; 

function_declaration
    : function_del
    ;

function_def
    : ID '(' formal_arguments_list_ ')' ':' scalar_type
    | ID '(' formal_arguments_list_ ')'
    ;

function_definition
    : function_def
      compound_statement
      END
    ;

function 
    : function_declaration
    | function_definition
    ;

function_list
    : 
    | function_list function
    ;

formal_arguments
    : identifier_list ':' type
    ;

formal_arguments_list
    : formal_arguments
    | formal_arguments_list ';' formal_arguments
    ;

formal_arguments_list_
    :
    | formal_arguments_list
    ;

identifier_list
    : ID
    | identifier_list ',' ID
    ;

variable_declaration
    : VAR identifier_list ':' type ';'
    ;

constant_declaration
    : VAR identifier_list ':' literal_constant ';'
    ;

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

scalar_type
    : INTEGER
    | REAL
    | STRING
    | BOOLEAN
    ;

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

literal_constant
    : integer_constant
    | '-' integer_constant
    | real_constant
    | '-' real_constant
    | string_literal
    | T
    | F
    ;


statement
    : compound_statement
    | simple_statement
    | conditional_statement
    | while_statement
    | for_statement
    | return_statement
    | function_call
    ;

statement_list
    : 
    | statement_list statement
    ;

compound_statement
    : START 
      declaration_list
      statement_list
      END
    ;

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

variable_reference
    : ID
    | ID array_reference
    ;

array_reference
    : '[' expression ']'
    | array_reference '[' expression ']'
    ;

conditional_statement
    : IF expression THEN compound_statement
      ELSE compound_statement
      END IF
    | IF expression THEN compound_statement
      END IF
    ;

while_statement
    : WHILE expression
      DO compound_statement
      END DO
    ;

for_statement
    : FOR ID ASSIGN integer_constant TO integer_constant
      DO compound_statement
      END DO
    ;

return_statement
    : RETURN expression ';'
    ;

function_call
    : ID '(' expression_list ')' ';'
    ;

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

expression_list
    :
    | expression_list_
    ;

expression_list_
    : expression
    | expression_list_ ',' expression
    ;

negative
    : '-' expression %prec '*'
    ;

arithmetic
    : expression '*' expression
    | expression '/' expression
    | expression '%' expression
    | expression '+' expression
    | expression '-' expression

relational
    : expression SMALLER expression
    | expression SMALLER_OR_EQUAL expression
    | expression SMALLER_OR_BIGGER expression
    | expression BIGGER_OR_EQUAL expression
    | expression BIGGER expression
    | expression EQUAL expression
    ;

logical
    : expression AND expression
    | expression OR expression
    | NOT expression
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    fclose(yyin);
    yylex_destroy();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
