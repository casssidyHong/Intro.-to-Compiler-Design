%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"
#include "AST/AstDumper.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <vector>
#include <cassert>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;    /* declareed in scanner.l */
extern char current_line[]; /* declareed in scanner.l */
extern FILE *yyin;          /* declareed by lex */
extern char *yytext;        /* declareed by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    class AstNode;
    struct Variables;
    struct NodeType;
    struct Operator;
    struct Id;
    enum class DataType;
    enum class OperatorType;   
    enum class BooleanType; 
    #include <vector>
}

%union { 
    char *yy_ID;
    std::vector <Id> *yy_id_list;
    AstNode *yy_node;
    std::vector <AstNode*> *yy_node_list;
    Variables *yy_var;
    int yy_int;
    double yy_real;
    NodeType *yy_node_type;
    std::vector <NodeType*> *yy_node_type_list;
};

%type <yy_ID> program_name ID STRING_LITERAL func_name
%type <yy_id_list> ID_list
%type <yy_var> type return_type scalar_type array_type int_real string_bool array_declartion literal_constant 
%type <yy_int> negative_not INT_LITERAL
%type <yy_real> REAL_LITERAL
%type <yy_node> compound_statement declare function_declare function_definition function 
%type <yy_node> stat simple condition while for return function_call expr func_invoc var_ref else_not 
%type <yy_node_list> declare_list statements statement_list declares
%type <yy_node_list> functions function_list exprs expression_list arr_ref_list arr_refs
%type <yy_node_type> formal_argument
%type <yy_node_type_list> formal_arguments formal_argument_list


%token COMMA SEMICOLON COLON
%token LPAREN RPAREN
%token LBRACK RBRACK

%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

%token ARRAY BOOLEAN INTEGER REAL STRING
%token BEGIN_ END
%token DO IF THEN WHILE ELSE FOR ID
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

%%

Program:
    program_name SEMICOLON
    declare_list function_list compound_statement
    END 
    {
        root = new ProgramNode(@1.first_line, @1.first_column, $1, "void", $3, $4, $5);
        free($1);
    }
;

program_name:
    ID 
;

declare_list:
    Epsilon { $$ = NULL; }
    |
    declares 
;

declares:
    declare
    {
        $$ = new std::vector <AstNode*> ();
        $$->push_back($1);
    }
    |
    declares declare
    {
        $1->push_back($2);
        $$ = $1;
    }
;

function_list:
    Epsilon { $$ = NULL; }
    |
    functions
;

functions:
    function
    {
        $$ = new std::vector <AstNode*>;
        $$->push_back($1);
    }
    |
    functions function
    {
        $1->push_back($2);
        $$ = $1;
    }
;

function:
    function_declare 
    
    |
    function_definition 
    
;

function_declare:
    func_name LPAREN formal_argument_list RPAREN return_type SEMICOLON
    {
        std::vector<Variables*> *param_list = new std::vector <Variables*>;
        std::vector <AstNode*> *decl_list = new std::vector <AstNode*>;

        if($3 != NULL){
            for(int i = 0; i < $3->size(); i++){ 
                for(int j = 0; j < $3->at(i)->cnt; j++){ 
                    Variables* var = new Variables;
                    var->type = $3->at(i)->type->type;
                    var->int_value = $3->at(i)->type->int_value;
                    var->real_value = $3->at(i)->type->real_value;
                    var->string_value = $3->at(i)->type->string_value;
                    var->boolean_type = $3->at(i)->type->boolean_type;
                    var->arr_num = $3->at(i)->type->arr_num;
                    param_list->push_back(var);
                }
                decl_list->push_back($3->at(i)->ast_node);
            }
        }
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, decl_list, param_list, $5, NULL);    
    }
;

function_definition:
    func_name LPAREN formal_argument_list RPAREN return_type
    compound_statement
    END
    {
        std::vector<Variables*> *param_list = new std::vector <Variables*>;
        std::vector <AstNode*> *decl_list = new std::vector <AstNode*>;

        if($3 != NULL){
            for(int i = 0; i < $3->size(); i++){    
                for(int j = 0; j < $3->at(i)->cnt; j++){ 
                    Variables* var = new Variables;
                    var->type = $3->at(i)->type->type;
                    var->int_value = $3->at(i)->type->int_value;
                    var->real_value = $3->at(i)->type->real_value;
                    var->string_value = $3->at(i)->type->string_value;
                    var->boolean_type = $3->at(i)->type->boolean_type;
                    var->arr_num = $3->at(i)->type->arr_num;

                    param_list->push_back(var);
                }
                decl_list->push_back($3->at(i)->ast_node);
            }
        }


        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, decl_list, param_list, $5, $6);  
    }
;

func_name:
    ID 
    
;

formal_argument_list:
    Epsilon { $$ = NULL; }
    |
    formal_arguments
;

formal_arguments:
    formal_argument
    {
        $$ = new std::vector <NodeType*>;
        $$->push_back($1);
    }
    |
    formal_arguments SEMICOLON formal_argument
    {
        $1->push_back($3);
        $$ = $1;
    }
;

formal_argument:
    ID_list COLON type
    {
        $$ = new NodeType;
        $$->type = $3;
        $$->cnt = 0;

        std::vector<AstNode*> *var_list = new std::vector <AstNode*>;
        for(int i = 0; i < $1->size(); i++){
            Variables *var = new Variables;
            var->type = $3->type;
            var->int_value = $3->int_value;
            var->real_value = $3->real_value;
            var->string_value = $3->string_value;
            var->boolean_type = $3->boolean_type;
            var->arr_num = $3->arr_num;           

            VariableNode *var_node = new VariableNode($1->at(i).line_number, $1->at(i).col_number, $1->at(i).name, var, NULL); 
            var_list->push_back(var_node);
            $$->cnt++;
        }        
        
        $$->ast_node = new DeclNode(@1.first_line, @1.first_column, var_list); 
    }
;

ID_list:
    ID
    {
        $$ = new std::vector <Id>;

        Id id_tmp;
        id_tmp.name = $1;
        id_tmp.line_number = @1.first_line;
        id_tmp.col_number = @1.first_column;
        
        $$->push_back(id_tmp);
    }
    |
    ID_list COMMA ID
    {
        Id id_tmp;
        id_tmp.name = $3;
        id_tmp.line_number = @3.first_line;
        id_tmp.col_number = @3.first_column;

        $1->push_back(id_tmp);

        $$ = $1;
    }
;

return_type:
    COLON scalar_type { $$ = $2; }
    |
    Epsilon
    {
        $$ = new Variables;
        $$->type = DataType::void_;
    }
;


declare:
    VAR ID_list COLON type SEMICOLON
    {
        std::vector <AstNode*> *var_list = new std::vector <AstNode*>;      
        for(int i = 0; i < $2->size(); i++){
            Variables* var_tmp = new Variables;
            var_tmp->type = $4->type;
            var_tmp->int_value = $4->int_value;
            var_tmp->real_value = $4->real_value;
            var_tmp->string_value = $4->string_value;
            var_tmp->boolean_type = $4->boolean_type;
            var_tmp->arr_num = $4->arr_num;
            
            VariableNode* variable_node = new VariableNode($2->at(i).line_number, $2->at(i).col_number, $2->at(i).name, var_tmp, NULL);

            var_list->push_back(variable_node);
        }               

        $$ = new DeclNode(@1.first_line, @1.first_column, var_list);
    }
    |
    VAR ID_list COLON literal_constant SEMICOLON
    {
        std::vector <AstNode*> *var_list = new std::vector <AstNode*>;     
        for(int i = 0; i < $2->size(); i++){
            Variables* var_tmp = new Variables;
            var_tmp->type = $4->type;
            var_tmp->int_value = $4->int_value;
            var_tmp->real_value = $4->real_value;
            var_tmp->string_value = $4->string_value;
            var_tmp->boolean_type = $4->boolean_type;
            var_tmp->arr_num = $4->arr_num;        

            ConstantValueNode* constant_value_node = new ConstantValueNode($4->line_number, $4->col_number, var_tmp);

            VariableNode* variable_node = new VariableNode($2->at(i).line_number, $2->at(i).col_number, $2->at(i).name, var_tmp, constant_value_node);
            
            var_list->push_back(variable_node);        
        }               
        $$ = new DeclNode(@1.first_line, @1.first_column, var_list);
    }
;

type:
    scalar_type { $$ = $1; }
    |
    array_type { $$ = $1; }
;

scalar_type:
    BOOLEAN
    {
        $$ = new Variables;
        $$->type = DataType::bool_type;
    }
    |
    INTEGER
    {
        $$ = new Variables;
        $$->type = DataType::int_type;
    }
    |
    STRING
    {
        $$ = new Variables;
        $$->type = DataType::string_type;
    }
    |
    REAL
    {
        $$ = new Variables;
        $$->type = DataType::real_type;
    }
;

array_type:
    array_declartion scalar_type
    {
        $$ = new Variables;
        $$->type = $2->type;
        $$->arr_num = $1->arr_num;
    }
;

array_declartion:
    ARRAY INT_LITERAL OF
    {
        $$ = new Variables;
        $$->arr_num.push_back($2);
    }
    |
    array_declartion ARRAY INT_LITERAL OF
    {
        $1->arr_num.push_back($3);
        $$ = $1;
    }
;

literal_constant:
    negative_not INT_LITERAL
    {
        $$ = new Variables;
        $$->type = DataType::int_type;
        $$->int_value = $1 * $2;

        if($1 == 1){
            $$->line_number = @2.first_line;
            $$->col_number = @2.first_column;                      
        }
        if($1 == -1){
            $$->line_number = @1.first_line;
            $$->col_number = @1.first_column;
        }
    }
    |
    negative_not REAL_LITERAL
    {
        $$ = new Variables;
        $$->type = DataType::real_type;
        $$->real_value = $1 * $2;

        if($1 == 1){
            $$->line_number = @2.first_line;
            $$->col_number = @2.first_column;  
        }
        if($1 == -1){
            $$->line_number = @1.first_line;
            $$->col_number = @1.first_column;
        }
    }
    |
    string_bool
    {
        $$ = $1;
    }
;

negative_not:
    Epsilon 
    { $$ = 1; }
    |
    MINUS %prec UNARY_MINUS 
    { $$ = -1; }
;

string_bool:
    STRING_LITERAL
    {
        $$ = new Variables;
        $$->string_value = $1;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->type = DataType::string_type;
    }
    |
    TRUE
    {
        $$ = new Variables;
        $$->boolean_type = BooleanType::true_bool;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->type = DataType::bool_type;
    }
    |
    FALSE
    {
        $$ = new Variables;
        $$->boolean_type = BooleanType::false_bool;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->type = DataType::bool_type;
    }
;

int_real:
    INT_LITERAL
    {
        $$ = new Variables;
        $$->int_value = $1;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->type = DataType::int_type;
    }
    |
    REAL_LITERAL
    {
        $$ = new Variables;
        $$->real_value = $1;
        $$->line_number = @1.first_line;
        $$->col_number = @1.first_column;
        $$->type = DataType::real_type;
    }
;

stat:
    compound_statement
    |
    simple
    |
    condition
    |
    while
    |
    for
    |
    return
    |
    function_call
;

compound_statement:
    BEGIN_
    declare_list
    statement_list
    END
    {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

simple:
    var_ref ASSIGN expr SEMICOLON { $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3); }
    |
    PRINT expr SEMICOLON { $$ = new PrintNode(@1.first_line, @1.first_column, $2); }
    |
    READ var_ref SEMICOLON { $$ = new ReadNode(@1.first_line, @1.first_column, $2); }
;

var_ref:
    ID arr_ref_list { $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2); }
;

arr_ref_list:
    Epsilon { $$ = NULL; }
    |
    arr_refs 
    
;

arr_refs:
    LBRACK expr RBRACK
    {
        $$ = new std::vector<AstNode*>;
        $$->push_back($2);
    }
    |
    arr_refs LBRACK expr RBRACK
    {
        $$ = $1;
        $1->push_back($3);
    }
;

condition:
    IF expr THEN
    compound_statement
    else_not
    END IF
    {
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

else_not:
    ELSE
    compound_statement { $$ = $2; }
    |
    Epsilon { $$ = NULL; }
;

while:
    WHILE expr DO
    compound_statement
    END DO { $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4); }
;

for:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    compound_statement
    END DO
    {
        /* declare Node */
        Variables* variable = new Variables;
        variable->type = DataType::int_type;
        VariableNode* variable_node = new VariableNode(@2.first_line, @2.first_column, $2, variable, NULL);

        std::vector<AstNode*> *variable_list = new std::vector<AstNode*>;
        variable_list->push_back(variable_node);
        DeclNode* declareation_node = new DeclNode(@2.first_line, @2.first_column, variable_list);


        /* Assignment Node */
        VariableReferenceNode* variable_reference_node = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
        Variables* variable_2 = new Variables;
        variable_2->type = DataType::int_type;
        variable_2->int_value = $4;
        ConstantValueNode* constant_value_node_1 = new ConstantValueNode(@4.first_line, @4.first_column, variable_2);

        AssignmentNode* assignment_node = new AssignmentNode(@3.first_line, @3.first_column, variable_reference_node, constant_value_node_1);
       

        /* Expression Node */
        Variables* variable_3 = new Variables;
        variable_3->type = DataType::int_type;
        variable_3->int_value = $6;
        ConstantValueNode* constant_value_node_2 = new ConstantValueNode(@6.first_line, @6.first_column, variable_3);
       
        /* for Node */
        $$ = new ForNode(@1.first_line, @1.first_column, declareation_node, assignment_node, constant_value_node_2, $8); 
    }
;

return:
    RETURN expr SEMICOLON
    { $$ = new ReturnNode(@1.first_line, @1.first_column, $2); }
;

function_call:
    func_invoc SEMICOLON 
    
;

func_invoc:
    ID LPAREN expression_list RPAREN 
    { $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3); }
;

expression_list:
    Epsilon 
    { $$ = NULL; }
    |
    exprs
;

exprs:
    expr
    {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    exprs COMMA expr
    {
        $1->push_back($3);
        $$=$1;
    }
;

statement_list:
    statements 
    |
    Epsilon 
    { $$ = NULL; }
;

statements:
    stat
    {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    statements stat
    {
        $1->push_back($2);
        $$=$1;
    }
;

expr:
    LPAREN expr RPAREN 
    { $$ = $2; }
    |
    expr EQUAL expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::equal_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr NOT_EQUAL expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::not_equal_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr LESS expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::smaller_op;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr LESS_OR_EQUAL expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::smaller_or_equal;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr GREATER expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::bigger_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr GREATER_OR_EQUAL expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::bigger_or_equal;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr AND expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::and_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr OR expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::or_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    NOT expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::not_op;
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, op_information, $2);
    }
    |
    expr PLUS expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::plus_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    MINUS expr %prec UNARY_MINUS
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::minus_op;
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, op_information, $2);
    }
    |
    expr MINUS expr{
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::minus_op;
        
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr MULTIPLY expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::multiply_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr DIVIDE expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::devide_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    expr MOD expr
    {
        Operator *op_information = new Operator;
        op_information->operator_type = OperatorType::mod_op;
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, op_information, $1, $3);
    }
    |
    int_real
    { $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1); }
    |
    string_bool
    { $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1); }
    |
    var_ref
    |
    func_invoc
;

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
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}