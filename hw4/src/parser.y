%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/AstDumper.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolManager.hpp"
#include "sema/ErrorMessageManager.hpp"

#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <typeinfo>
#include <vector>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */
extern uint32_t opt_sym;

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

static AstDumper ast_dump;
%}

%code requires {
    class ConstValue;
    class AstNode;
    class ProgramNode;
    class DeclNode;
    class FunctionNode;
    class CompoundStatementNode;
    class VariableNode;
    class ConstantValueNode;
    class AstNodeVisitor;
    class AstDumper;
    class PrintNode;
    class ExpressionNode;
    class BinaryOperatorNode;
    class UnaryOperatorNode;
    class FunctionInvocationNode;
    class VariableReferenceNode;
    class AssignmentNode;
    class ReadNode;
    class IfNode;
    class WhileNode;
    class ForNode;
    class ReturnNode;

    class SymbolManager;
    class SymbolTable;
    class SymbolEntry;
}

    /* For yylval */
%union {
    /* basic semantic value */
    int val;
    double dval;
    const char* text;
    bool boolean;

    const char* identifier;
    const char* type;

    AstNode* node;
    ProgramNode* prog_node;
    DeclNode* decl_node;
    FunctionNode* funct_node;
    CompoundStatementNode* cmpdstmt_node;
    VariableNode* var_node;
    ConstantValueNode* const_node;
    ExpressionNode* expr_node;
    BinaryOperatorNode* bin_op_node;
    UnaryOperatorNode* unary_op_node;
    FunctionInvocationNode* funct_invoc_node;
    PrintNode* print_node;
    VariableReferenceNode* var_ref_node;
    IfNode* if_node;
    WhileNode* while_node;
    ForNode* for_node;
    ReturnNode* return_node;
};

%type <identifier> ProgramName ID FunctionName
%type <const_node> LiteralConstant StringAndBoolean
%type <var_node> IdList 
%type <prog_node> Program 
%type <decl_node> DeclarationList Declarations Declaration FormalArg FormalArgList FormalArgs
%type <funct_node> FunctionList Function Functions FunctionDefinition FunctionDeclaration
%type <cmpdstmt_node> CompoundStatement ElseOrNot
%type <funct_invoc_node> FunctionInvocation FunctionCall
%type <var_ref_node> VariableReference
%type <if_node> Condition
%type <while_node> While
%type <for_node> For
%type <return_node> Return

    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token <boolean> FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token <val> INT_LITERAL
%token <dval> REAL_LITERAL 
%token <text> STRING_LITERAL 

    /* Other non terminal */
%type <val> NegOrNot
%type <const_node> IntegerAndReal

    /* Type */
%type <type> Type ScalarType ArrType ReturnType
%type <type> ArrDecl

    /* Statement */
%type <node> Statement StatementList Statements
%type <node> Simple

    /* Expression */
%type <expr_node> ExpressionList Expressions Expression ArrRefList ArrRefs

%%

Program:
    ProgramName SEMICOLON
    DeclarationList 
    FunctionList 
    CompoundStatement 
    END {
        root = new ProgramNode(@1.first_line, @1.first_column, $1, "void", $3, $4, $5);
        // free($1);
        // symbol_manager.popScope();
    }
;

ProgramName:
    ID {
        $$ = $1;
    }
;

DeclarationList:
    Epsilon {
        $$ = nullptr;
        // printf("Decl: %s\n", $1);
    }
    |
    Declarations {
        $$ = $1;
    } 
;

Declarations:
    Declaration {
        $$ = $1;
    }
    |
    Declarations Declaration {
        DeclNode* new_decl = $2;
        new_decl->setNextNode($1);
        $$ = new_decl;
    }
;

FunctionList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Functions {
        $$ = $1;
    }
;

Functions:
    Function {
        $$ = $1;
    }
    |
    Functions Function {
        FunctionNode* new_head = $2;
        new_head->setNextNode($1);
        $$ = new_head;
    }
;

Function:
    FunctionDeclaration {
        $$ = $1;
    }
    |
    FunctionDefinition {
        $$ = $1;
    }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        FunctionNode* funct = new FunctionNode(@1.first_line, @1.first_column, $3, $5, $1);

        $$ = funct;
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        FunctionNode* funct = new FunctionNode(@1.first_line, @1.first_column, $3, $5, $1, $6, true);
        $$ = funct;
    }
;

FunctionName:
    ID {
        $$ = $1;
    }
;

FormalArgList:
    Epsilon {
        $$ = nullptr;
    }
    |
    FormalArgs {
        $$ = $1;
    }
;

FormalArgs:
    FormalArg {
        $$ = $1;
    }
    |
    FormalArgs SEMICOLON FormalArg {
        DeclNode* new_head = $3;
        new_head->setNextNode($1);
        $$ = new_head;
    }
;

FormalArg:
    IdList COLON Type {
        DeclNode* decl = new DeclNode(@1.first_line, @1.first_column, $3, $1, true);
        $$ = decl;
    }
;

IdList:
    ID {
        VariableNode* new_var = new VariableNode(@1.first_line, @1.first_column, $1);
        $$ = new_var;
    }
    |
    IdList COMMA ID {
        // printf("Create ID nodes\n");
        VariableNode* new_var = new VariableNode(@3.first_line, @3.first_column, $3);
        // printf("Created\n");
        new_var->setNextNode($1);
        $$ = new_var;
    }
;

ReturnType:
    COLON ScalarType {
        $$ = $2;
    }
    |
    Epsilon {
        const char* type = "void";
        $$ = type;
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        DeclNode* new_decl = new DeclNode(@1.first_line, @1.first_column, $4, $2, false);
        $$ = new_decl;
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        DeclNode* new_decl = new DeclNode(@1.first_line, @1.first_column, $4, $2);
        $$ = new_decl;
    }
;

Type:
    ScalarType {
        $$ = $1;
    }
    |
    ArrType {
        $$ = $1;
    }
;

ScalarType:
    INTEGER {
        $$ = "integer";
    }
    |
    REAL {
        $$ = "real";
    }
    |
    STRING {
        $$ = "string";
    }
    |
    BOOLEAN {
        $$ = "boolean";
    }
;

ArrType:
    ArrDecl ScalarType {
        char* arr_type = (char*)calloc(strlen($1)+strlen($2)+2, sizeof(char));
        strcpy(arr_type, $2);
        strcat(arr_type, " ");
        strcat(arr_type, $1);

        const char* ret_arr = arr_type;
        $$ = ret_arr;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        char* arr = (char*)calloc(10, sizeof(char));
        sprintf(arr, "[%d]", $2);

        const char* ret_arr = arr;
        $$ = ret_arr;
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        char* arr = (char*)calloc(10, sizeof(char));
        sprintf(arr, "[%d]", $3);
        
        char* new_arr = (char*)calloc(strlen($1)+10+1, sizeof(char));
        strcpy(new_arr, $1);
        strcat(new_arr, arr);

        const char* ret_arr = new_arr;
        $$ = ret_arr;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        int res = $2 * $1;
        ConstValue value;
        value.int_value = res;

        ConstantValueNode* new_const;

        if($1 == 1)
            new_const = new ConstantValueNode(@2.first_line, @2.first_column, "integer", value);
        else new_const = new ConstantValueNode(@1.first_line, @1.first_column, "integer", value);

        $$ = new_const;
    }
    |
    NegOrNot REAL_LITERAL {
        double res = $2 * $1;
        ConstValue value;
        value.real_value = res;
        
        ConstantValueNode* new_const = new ConstantValueNode(@1.first_line, @1.first_column, "real", value);
        
        $$ = new_const;
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

NegOrNot:
    Epsilon {
        $$ = 1;
    }
    |
    MINUS %prec UNARY_MINUS {
        $$ = -1;
    }
;

StringAndBoolean:
    STRING_LITERAL {
        const char* ret_str = $1;

        ConstValue value;
        value.str_value = ret_str;

        ConstantValueNode* new_const = new ConstantValueNode(@1.first_line, @1.first_column, "string", value);
        $$ = new_const;
    }
    |
    TRUE {
        // const char* str = "true";

        ConstValue value;
        value.bool_value = $1;

        ConstantValueNode* new_const = new ConstantValueNode(@1.first_line, @1.first_column, "boolean", value);
        $$ = new_const;
    }
    |
    FALSE {
        // const char* str = "false";

        ConstValue value;
        value.bool_value = $1;

        ConstantValueNode* new_const = new ConstantValueNode(@1.first_line, @1.first_column, "boolean", value);
        $$ = new_const;
    }
;

IntegerAndReal:
    INT_LITERAL {
        ConstValue value;
        value.int_value = $1;

        ConstantValueNode* value_node = new ConstantValueNode(@1.first_line, @1.first_column, "integer", value);
        $$ = value_node;
    }
    |
    REAL_LITERAL {
        ConstValue value;
        value.real_value = $1;

        ConstantValueNode* value_node = new ConstantValueNode(@1.first_line, @1.first_column, "real", value);
        $$ = value_node;
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement {
        $$ = $1;
    }
    |
    Simple {
        $$ = $1;
    }
    |
    Condition {
        $$ = $1;
    }
    |
    While {
        $$ = $1;
    }
    |
    For {
        $$ = $1;
        // printf("get for\n");
    }
    |
    Return {
        $$ = $1;
    }
    |
    FunctionCall {
        $$ = $1;
        // printf("get function call\n"); 
    }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        AssignmentNode* assign_node = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
        $$ = assign_node;
    }
    |
    PRINT Expression SEMICOLON {
        PrintNode* print_node = new PrintNode(@1.first_line, @1.first_column, $2);
        $$ = print_node;

        // printf("get print!!\n");
    }
    |
    READ VariableReference SEMICOLON {
        ReadNode* read_node = new ReadNode(@1.first_line, @1.first_column, $2);
        $$ = read_node;
    }
;

VariableReference:
    ID ArrRefList {
        VariableReferenceNode* ref_node = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
        $$ = ref_node;
    }
;

ArrRefList:
    Epsilon {
        $$ = nullptr;
    }
    |
    ArrRefs {
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        ExpressionNode* expr_node = $2;
        $$ = expr_node;
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        ExpressionNode* new_head = $3;
        new_head->setNextNode($1);
        $$ = new_head;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        IfNode* if_stmt = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
        $$ = if_stmt;
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = nullptr;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        WhileNode* while_stmt = new WhileNode(@1.first_line, @1.first_column, $2, $4);
        $$ = while_stmt;
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        VariableNode* var = new VariableNode(@2.first_line, @2.first_column, $2);
        var->setVarType("integer");
        var->setLoopVar(true);

        ConstValue init_value;
        init_value.int_value = $4;

        ConstValue final_value;
        final_value.int_value = $6;

        ConstantValueNode* init_const = new ConstantValueNode(@4.first_line, @4.first_column, "integer", init_value);
        ConstantValueNode* final_const = new ConstantValueNode(@6.first_line, @6.first_column, "integer", final_value);

        DeclNode* decl = new DeclNode(@2.first_line, @2.first_column, "integer", var, false);

        VariableReferenceNode* var_ref = new VariableReferenceNode(@2.first_line, @2.first_column, $2);

        AssignmentNode* assignment = new AssignmentNode(@3.first_line, @3.first_column, var_ref, init_const);

        ForNode* for_stmt = new ForNode(@1.first_line, @1.first_column, decl, assignment, final_const, $8);

        $$ = for_stmt;
    }
;

Return:
    RETURN Expression SEMICOLON {
        ReturnNode* return_stmt = new ReturnNode(@1.first_line, @1.first_column, $2);
        $$ = return_stmt;
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON {
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        FunctionInvocationNode* invoc_node = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
        $$ = invoc_node;
    }
;

ExpressionList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Expressions {
        $$ = $1;
    }
;

Expressions:
    Expression {
        $$ = $1;
    }
    |
    Expressions COMMA Expression {
        ExpressionNode* new_head = $3;
        new_head->setNextNode($1);
        $$ = new_head;
    }
;

StatementList:
    Epsilon {
        $$ = nullptr;
    }
    |
    Statements {
        $$ = $1;
    }
;

Statements:
    Statement {
        $$ = $1;
    }
    |
    Statements Statement {
        AstNode* new_head = $2;
        new_head->setNextNode($1);
        $$ = new_head;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UNARY_MINUS {
        UnaryOperatorNode* un_op = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2);
        $$ = un_op;
    }
    |
    Expression MULTIPLY Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", $1, $3);
        $$ = bin_op;
    }
    |
    Expression DIVIDE Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", $1, $3);
        $$ = bin_op;
    }
    |
    Expression MOD Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", $1, $3);
        $$ = bin_op;
    }
    |
    Expression PLUS Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3);
        $$ = bin_op;
    }
    |
    Expression MINUS Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", $1, $3);
        $$ = bin_op;
    }
    |
    Expression LESS Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", $1, $3);
        $$ = bin_op;
    }
    |
    Expression LESS_OR_EQUAL Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", $1, $3);
        $$ = bin_op;
    }
    |
    Expression GREATER Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", $1, $3);
        $$ = bin_op;
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", $1, $3);
        $$ = bin_op;
    }
    |
    Expression EQUAL Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", $1, $3);
        $$ = bin_op;
    }
    |
    Expression NOT_EQUAL Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", $1, $3);
        $$ = bin_op;
    }
    |
    NOT Expression {
        UnaryOperatorNode* un_op = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2);
        $$ = un_op;
    }
    |
    Expression AND Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3);
        $$ = bin_op;
    }
    |
    Expression OR Expression {
        BinaryOperatorNode* bin_op = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", $1, $3);
        $$ = bin_op;
    }
    |
    IntegerAndReal {
        $$ = $1;
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
    |
    VariableReference {
        $$ = $1;
    }
    |
    FunctionInvocation {
        $$ = $1;
    }
;

    /*
       misc
            */
Epsilon:
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
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r"); 
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    FILE* fp = fopen(argv[1], "r");
    
    if(fp == NULL){
        perror("fopen() failed:");
    }

    std::vector<char*> code;

    char line[256];
    while(fgets(line, sizeof(line), fp) != NULL){
        char* new_line = (char*)calloc(256, sizeof(char));
        strcpy(new_line, line);
        code.push_back(new_line);
    }

    fclose(fp);

    SemanticAnalyzer sema_analyzer(code);

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        // root->print();
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    // table dump
    
    root->accept(sema_analyzer);

    if(sema_analyzer.hasError()){
        sema_analyzer.showError();
    }else{
        printf("\n"
           "|---------------------------------------------------|\n"
           "|  There is no syntactic error and semantic error!  |\n"
           "|---------------------------------------------------|\n");
    }
    

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
