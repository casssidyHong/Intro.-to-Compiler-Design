#include "sema/ErrorMessageManager.hpp"

ErrorMsg::ErrorMsg(AstNode* err_node, int err, char* err_line, const char* sym_name){
    this->err_node = err_node;
    this->err = err;
    this->err_line = err_line;
    this->sym_name = sym_name;
}

void ErrorMsg::setErr(int err){
    this->err = err;
}

int ErrorMsg::getErr(){
    return this->err;
}

void ErrorMsg::setSymName(const char* sym_name){
    this->sym_name = sym_name;
}

const char* ErrorMsg::getSymName(){
    return this->sym_name;
}

void ErrorMsg::setErrorNode(AstNode* err_node){
    this->err_node = err_node;
}

AstNode* ErrorMsg::getErrorNode(){
    return this->err_node;
}

void ErrorMsg::showErrMsg(){

    VariableReferenceNode* err_var_ref;
    BinaryOperatorNode* err_bin_op;
    UnaryOperatorNode* err_un_op;
    FunctionInvocationNode* err_funct_invoc;
    FunctionNode* funct;
    PrintNode* print_node;
    ReadNode* read_node;
    AssignmentNode* assign_node;
    IfNode* if_node;
    ForNode* for_node;
    ReturnNode* return_node;

    char* param_type;
    char* param_tok;
    bool first = true;
    char* tmp_param_tok;

    switch(this->err){
        case REDECL_ERROR:
            fprintf(stderr, "<Error> Found in line %d, column %d: symbol \'%s\' is redeclared\n", this->err_node->getLocation().line, this->err_node->getLocation().col,
                        this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case ARR_DIM_ZERO:
            fprintf(stderr, "<Error> Found in line %d, column %d: \'%s\' declared as an array with an index that is not greater than 0\n", 
                        this->err_node->getLocation().line, this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case SYM_NOT_FOUND:
            fprintf(stderr, "<Error> Found in line %d, column %d: use of undeclared symbol \'%s\'\n", 
                        this->err_node->getLocation().line, this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case SYM_NOT_VAR:
            fprintf(stderr, "<Error> Found in line %d, column %d: use of non-variable symbol \'%s\'\n", 
                        this->err_node->getLocation().line, this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case DIM_NOT_INT: 

            err_var_ref = dynamic_cast<VariableReferenceNode*>(this->err_node);
            
            for(ExpressionNode* expr : err_var_ref->getExprs()){
                if(strcmp(expr->getType(), "integer") != 0){
                    fprintf(stderr, "<Error> Found in line %d, column %d: index of array reference must be an integer\n", 
                        expr->getLocation().line, expr->getLocation().col);
                    fprintf(stderr, "    %s", this->err_line);
                    fprintf(stderr, "    ");

                    for(int i = 1; i < expr->getLocation().col; i++){
                        fprintf(stderr, " ");
                    }

                    fprintf(stderr, "^\n");
                }
            }

            break;

        case OVER_ARR_SUBSCRIPT:
            fprintf(stderr, "<Error> Found in line %d, column %d: there is an over array subscript on \'%s\'\n", 
                        this->err_node->getLocation().line, this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case INVALID_BIN_OP:

            err_bin_op = dynamic_cast<BinaryOperatorNode*>(this->err_node);

            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operands to binary operator \'%s\' (\'%s\' and \'%s\')\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col, this->sym_name, err_bin_op->getFrontExpr()->getType(), err_bin_op->getBackExpr()->getType());
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case INVALID_UN_OP:

            err_un_op = dynamic_cast<UnaryOperatorNode*>(this->err_node);

            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operand to unary operator \'%s\' (\'%s\')\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col, this->sym_name, err_un_op->getExpr()->getType());
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case FUNCT_NOT_FOUND:
            
            fprintf(stderr, "<Error> Found in line %d, column %d: use of undeclared symbol \'%s\'\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case NON_FUNCT_CALL:
            
            fprintf(stderr, "<Error> Found in line %d, column %d: call of non-function symbol \'%s\'\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case INVALID_ARG_NUM:

            fprintf(stderr, "<Error> Found in line %d, column %d: too few/much arguments provided for function \'%s\'\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col, this->sym_name);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " ");
            }

            fprintf(stderr, "^\n");
            break;

        case ARG_TYPE_UNMATCH:

            err_funct_invoc = dynamic_cast<FunctionInvocationNode*>(this->err_node);
            param_type = strdup(err_funct_invoc->getParamType());
            // printf("param_type: %s\n", param_type);
            param_tok = strtok(param_type, ",");

            for(ExpressionNode* expr : err_funct_invoc->getExprs()){
                
                // printf("%s vs %s\n", expr->getType(), param_tok);

                if(first){
                    tmp_param_tok = strdup(param_tok);
                    first = false;
                }else tmp_param_tok = strdup(param_tok+1);

                // printf("%s vs %s\n", expr->getType(), tmp_param_tok);

                if(strcmp(expr->getType(), tmp_param_tok) == 0){
                    param_tok = strtok(NULL, ",");
                    continue;
                }
                    
                fprintf(stderr, "<Error> Found in line %d, column %d: incompatible type passing \'%s\' to parameter of type \'%s\'\n", 
                        expr->getLocation().line, expr->getLocation().col, expr->getType(), tmp_param_tok);
                fprintf(stderr, "    %s", this->err_line);
                fprintf(stderr, "    ");

                for(int i = 1; i < expr->getLocation().col; i++){
                    fprintf(stderr, " ");
                }

                fprintf(stderr, "^\n");

                param_tok = strtok(NULL, ",");
            }
            
            break;

        case PRINT_NONSCALAR:

            print_node = dynamic_cast<PrintNode*>(this->err_node);

            fprintf(stderr, "<Error> Found in line %d, column %d: expression of print statement must be scalar type\n", print_node->getExpr()->getLocation().line, 
                    print_node->getExpr()->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < print_node->getExpr()->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case READ_NONSCALAR:

            read_node = dynamic_cast<ReadNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: variable reference of read statement must be scalar type\n", read_node->getVarRef()->getLocation().line, 
                    read_node->getVarRef()->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < read_node->getVarRef()->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case READ_VAR_REF_ERR:

            read_node = dynamic_cast<ReadNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: variable reference of read statement cannot be a constant or loop variable\n", read_node->getVarRef()->getLocation().line, 
                    read_node->getVarRef()->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < read_node->getVarRef()->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case ARR_ASSIGN:

            // assign_node = dynamic_cast<AssignmentNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: array assignment is not allowed\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case CONST_ASSIGN:

            assign_node = dynamic_cast<AssignmentNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: cannot assign to variable \'%s\' which is a constant\n", assign_node->getLocation().line, 
                    assign_node->getVarRef()->getLocation().col, assign_node->getVarRef()->getRefName());
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < assign_node->getVarRef()->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case LOOP_VAR_ASSIGN:

            fprintf(stderr, "<Error> Found in line %d, column %d: the value of loop variable cannot be modified inside the loop body\n", this->err_node->getLocation().line, 
                    this->err_node->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < this->err_node->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case ASSIGN_TYPE_ERROR:

            assign_node = dynamic_cast<AssignmentNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: assigning to \'%s\' from incompatible type \'%s\'\n", assign_node->getLocation().line, 
                    assign_node->getLocation().col, assign_node->getVarRef()->getType(), assign_node->getExpr()->getType());
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < assign_node->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case CONDITION_NOT_BOOL:

            if_node = dynamic_cast<IfNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: the expression of condition must be boolean type\n", if_node->getCond()->getLocation().line, 
                    if_node->getCond()->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < if_node->getCond()->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case LOOP_VAR_DEC:

            for_node = dynamic_cast<ForNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: the lower bound and upper bound of iteration count must be in the incremental order\n", 
                    for_node->getLocation().line, for_node->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < for_node->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case RET_TYPE_ERROR:

            return_node = dynamic_cast<ReturnNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: return \'%s\' from a function with return type \'%s\'\n", return_node->getExpr()->getLocation().line, 
                    return_node->getExpr()->getLocation().col, return_node->getExpr()->getType(), return_node->getReturnType());
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < return_node->getExpr()->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;

        case RET_POS_ERROR:

            return_node = dynamic_cast<ReturnNode*>(this->err_node);
            fprintf(stderr, "<Error> Found in line %d, column %d: program/procedure should not return a value\n", 
                    return_node->getLocation().line, return_node->getLocation().col);
            fprintf(stderr, "    %s", this->err_line);
            fprintf(stderr, "    ");

            for(int i = 1; i < return_node->getLocation().col; i++){
                fprintf(stderr, " "); 
            }

            fprintf(stderr, "^\n");
            break;
    }
}

void ErrorManager::addErrorMsg(AstNode* err_node, int err, int row, const char* sym_name){
    ErrorMsg new_msg = ErrorMsg(err_node, err, this->code[row], sym_name);
    this->err_list.push_back(new_msg);
}

void ErrorManager::showErrList(){
    for(ErrorMsg err_msg : this->err_list){
        err_msg.showErrMsg();
    }
}

void ErrorManager::addCodeLine(char* line){
    this->code.push_back(line);
}

char* ErrorManager::getCodeLine(int row){
    return this->code[row];
}

void ErrorManager::setCode(std::vector<char*> code){
    this->code = code;
}

std::vector<char*>& ErrorManager::getCode(){
    return this->code;
}

bool ErrorManager::isDupErr(const char* name){
    for(ErrorMsg err_msg : this->err_list){
        if(strcmp(name, err_msg.getSymName()) == 0 && (err_msg.getErr() == ARR_DIM_ZERO || err_msg.getErr() == REDECL_ERROR || 
                    err_msg.getErr() == SYM_NOT_FOUND))
            return true;
    }

    return false;
}