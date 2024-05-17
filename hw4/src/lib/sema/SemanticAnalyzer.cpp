#include "sema/SemanticAnalyzer.hpp"

SemanticAnalyzer::SemanticAnalyzer(std::vector<char*> code){
    this->err_man.setCode(code); 
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    SymbolTable* symtab = new SymbolTable();
    this->sym_man.pushScope(symtab);
    
    SymbolTable* cur_symtab = this->sym_man.getCurSymTab();
    cur_symtab->addSymbol(p_program.getProgName(), "program", this->sym_man.getCurLevel(), "void", &p_program);

    p_program.visitChildNodes(*this);

    this->sym_man.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {

    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    // printf("name: %s, isParam: %d\n", p_variable.getVarName(), p_variable.isParam());

    if(this->sym_man.isRedecl(p_variable.getVarName())){
        // printf("aaa\n");
        this->has_error = true;
        this->err_man.addErrorMsg(&p_variable, REDECL_ERROR, p_variable.getLocation().line-1, p_variable.getVarName());
        return;
    }else if(this->sym_man.isErrorDim(p_variable.getVarType())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_variable, ARR_DIM_ZERO, p_variable.getLocation().line-1, p_variable.getVarName());
    }

    SymbolTable* cur_symtab = this->sym_man.getCurSymTab();

    if(p_variable.isAssigned()){
        cur_symtab->addSymbol(p_variable.getVarName(), "constant", this->sym_man.getCurLevel(), p_variable.getVarType(), p_variable.getValueNode());        
        p_variable.visitChildNodes(*this);
    }else if(p_variable.isParam()){
        // printf("bbb\n");
        cur_symtab->addSymbol(p_variable.getVarName(), "parameter", this->sym_man.getCurLevel(), p_variable.getVarType(), &p_variable);
    }else if(p_variable.isLoopVar()){
        cur_symtab->addSymbol(p_variable.getVarName(), "loop_var", this->sym_man.getCurLevel(), p_variable.getVarType(), &p_variable);
    }else{
        cur_symtab->addSymbol(p_variable.getVarName(), "variable", this->sym_man.getCurLevel(), p_variable.getVarType(), &p_variable);
    }
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    if(this->sym_man.isRedecl(p_function.getFunctName())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_function, REDECL_ERROR, p_function.getLocation().line-1, p_function.getFunctName());

        SymbolTable* empty_symtab = new SymbolTable();
        this->sym_man.pushScope(empty_symtab);
        p_function.visitChildNodes(*this);
        this->sym_man.popScope();

        if(!this->sym_man.isFunctStackEmpty())
            this->sym_man.popFunctStack();

    }else{
        SymbolTable* cur_symtab = this->sym_man.getCurSymTab();
        cur_symtab->addSymbol(p_function.getFunctName(), "function", this->sym_man.getCurLevel(), p_function.getReturnType(), &p_function);

        if(strcmp(p_function.getReturnType(), "void") != 0)
            this->sym_man.pushFunctStack(strdup(p_function.getFunctName()));

        SymbolTable* new_symtab = new SymbolTable();
        // printf("function!!\n");
        this->sym_man.pushScope(new_symtab);
        p_function.visitChildNodes(*this);
        this->sym_man.popScope();

        if(!this->sym_man.isFunctStackEmpty())
            this->sym_man.popFunctStack();
    }
    
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    if(!p_compound_statement.isFunct()){
        SymbolTable* new_symtab = new SymbolTable();
        this->sym_man.pushScope(new_symtab);
        // printf("compound!!\n");
        p_compound_statement.visitChildNodes(*this);
        this->sym_man.popScope();
    }else{
        p_compound_statement.visitChildNodes(*this);
    }

}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    // printf("print!!!!\n");
    p_print.visitChildNodes(*this);
    
    // printf("type: %s\n", p_print.getExpr()->getType());
    if(this->sym_man.isNotScalar(p_print.getExpr()->getType()) && strcmp(p_print.getExpr()->getType(), "error-type") != 0){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_print, PRINT_NONSCALAR, p_print.getLocation().line-1, "print");
    }

}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_bin_op.visitChildNodes(*this);

    if(this->sym_man.invalidBinOp(p_bin_op)){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_bin_op, INVALID_BIN_OP, p_bin_op.getLocation().line-1, p_bin_op.getOp());
        p_bin_op.setType("error-type");
        return;
    }

    if(strcmp(p_bin_op.getOp(), "<>") == 0 || strcmp(p_bin_op.getOp(), "<=") == 0 || strcmp(p_bin_op.getOp(), ">=") == 0 || strcmp(p_bin_op.getOp(), "=") == 0 || 
            strcmp(p_bin_op.getOp(), "<") == 0 || strcmp(p_bin_op.getOp(), ">") == 0){

        p_bin_op.setType("boolean");

    }else if(strcmp(p_bin_op.getFrontExpr()->getType(), p_bin_op.getBackExpr()->getType()) != 0 && (strcmp(p_bin_op.getFrontExpr()->getType(), "integer") == 0 ||
            strcmp(p_bin_op.getFrontExpr()->getType(), "real") == 0)){

        p_bin_op.setType("real");
    }else{

        p_bin_op.setType(p_bin_op.getFrontExpr()->getType());
    }
    
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_un_op.visitChildNodes(*this);

    if(this->sym_man.invalidUnOp(p_un_op)){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_un_op, INVALID_UN_OP, p_un_op.getLocation().line-1, p_un_op.getOp());
        p_un_op.setType("error-type");
        return;
    }

    p_un_op.setType(p_un_op.getExpr()->getType());
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    // printf("Invoc!!\n");
    // printf("name: %s\n", p_func_invocation.getFunctName());

    if(!this->sym_man.isFunctDecl(p_func_invocation.getFunctName())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_func_invocation, FUNCT_NOT_FOUND, p_func_invocation.getLocation().line-1, p_func_invocation.getFunctName());
        p_func_invocation.setType("error-type");
        return;
    }else if(!this->sym_man.isSymFunct(p_func_invocation.getFunctName())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_func_invocation, NON_FUNCT_CALL, p_func_invocation.getLocation().line-1, p_func_invocation.getFunctName());
        p_func_invocation.setType("error-type");
        return;
    }

    p_func_invocation.setParamType(this->sym_man.getFunctParamType(p_func_invocation.getFunctName()));
    p_func_invocation.setType(this->sym_man.getFunctReturnType(p_func_invocation.getFunctName()));
    p_func_invocation.visitChildNodes(*this);
    
    if(this->sym_man.invalidArgNum(p_func_invocation.getExprs().size(), p_func_invocation)){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_func_invocation, INVALID_ARG_NUM, p_func_invocation.getLocation().line-1, p_func_invocation.getFunctName());
        p_func_invocation.setType("error-type");
        // printf("function invocation check!\n");
        return;
    }else if(this->sym_man.argTypeNotMatch(p_func_invocation)){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_func_invocation, ARG_TYPE_UNMATCH, p_func_invocation.getLocation().line-1, p_func_invocation.getFunctName());
        p_func_invocation.setType("error-type");
        // printf("function invocation check stage two!\n");
        return;
    }

    const char* return_type = this->sym_man.getFunctInvocType(p_func_invocation.getFunctName());

    p_func_invocation.setType(return_type);
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // printf("var_ref name: %s, type: %s\n", p_variable_ref.getRefName(), p_variable_ref.getType());
    // SymbolTable* cur_symtab = this->sym_man.getCurSymTab();
    // for(SymbolEntry entry : cur_symtab->getEntries()){
    //     printf("var name: %s, type: %s\n", entry.getName(), entry.getType());
    // }

    // printf("Variable %s is referenced\n", p_variable_ref.getRefName());

    p_variable_ref.visitChildNodes(*this);

    if(this->sym_man.symNotFound(p_variable_ref.getRefName()) && !this->err_man.isDupErr(p_variable_ref.getRefName())){

        this->has_error = true;
        this->err_man.addErrorMsg(&p_variable_ref, SYM_NOT_FOUND, p_variable_ref.getLocation().line-1, p_variable_ref.getRefName());
        p_variable_ref.setType("error-type");
        return;

    }else if(this->sym_man.symNotVar(p_variable_ref.getRefName()) && !this->err_man.isDupErr(p_variable_ref.getRefName())){

        this->has_error = true;
        this->err_man.addErrorMsg(&p_variable_ref, SYM_NOT_VAR, p_variable_ref.getLocation().line-1, p_variable_ref.getRefName());
        p_variable_ref.setType("error-type");
        return;

    }else if(strcmp(p_variable_ref.getType(), "array") == 0 && this->sym_man.dimNotInt(p_variable_ref) 
                    && !this->err_man.isDupErr(p_variable_ref.getRefName()) && !this->sym_man.checkExprError(p_variable_ref.getExprs())){

        this->has_error = true;
        this->err_man.addErrorMsg(&p_variable_ref, DIM_NOT_INT, p_variable_ref.getLocation().line-1, p_variable_ref.getRefName());
        p_variable_ref.setType("error-type");
        return;

    }else if(strcmp(p_variable_ref.getType(), "array") == 0 && this->sym_man.overArrSubScript(p_variable_ref)
                    && !this->err_man.isDupErr(p_variable_ref.getRefName())){
                        
        this->has_error = true;
        this->err_man.addErrorMsg(&p_variable_ref, OVER_ARR_SUBSCRIPT, p_variable_ref.getLocation().line-1, p_variable_ref.getRefName());
        p_variable_ref.setType("error-type");
        return;

    }

    this->sym_man.setVarRefType(p_variable_ref);
    // printf("var ref type: %s\n", p_variable_ref.getType());
    
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_assignment.visitChildNodes(*this);
    
    if(this->sym_man.isConst(p_assignment.getVarRef()->getRefName())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_assignment, CONST_ASSIGN, p_assignment.getLocation().line-1, "assign");
    }else if(this->sym_man.isLoopVar(p_assignment.getVarRef()->getRefName()) && !this->err_man.isDupErr(p_assignment.getVarRef()->getRefName())){
        this->has_error = true;
        this->err_man.addErrorMsg(p_assignment.getVarRef(), LOOP_VAR_ASSIGN, p_assignment.getLocation().line-1, "assign");
    }else if(this->sym_man.isNotScalar(p_assignment.getVarRef()->getType()) && !this->err_man.isDupErr(p_assignment.getVarRef()->getRefName())){
        this->has_error = true;
        this->err_man.addErrorMsg(p_assignment.getVarRef(), ARR_ASSIGN, p_assignment.getLocation().line-1, "assign");
    }else if(this->sym_man.isNotScalar(p_assignment.getExpr()->getType()) && strcmp(p_assignment.getExpr()->getType(), "error-type") != 0){
        this->has_error = true;
        this->err_man.addErrorMsg(p_assignment.getExpr(), ARR_ASSIGN, p_assignment.getLocation().line-1, "assign");
    }else if(!this->sym_man.checkAssignType(&p_assignment) && !this->err_man.isDupErr(p_assignment.getVarRef()->getRefName())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_assignment, ASSIGN_TYPE_ERROR, p_assignment.getLocation().line-1, "assign");
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_read.visitChildNodes(*this);

    if(this->sym_man.isNotScalar(p_read.getVarRef()->getType()) && strcmp(p_read.getVarRef()->getType(), "error-type") != 0){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_read, READ_NONSCALAR, p_read.getLocation().line-1, "read");
    }else if(this->sym_man.isLoopVarOrContant(p_read.getVarRef()->getRefName())){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_read, READ_VAR_REF_ERR, p_read.getLocation().line-1, "read");
    }

}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_if.visitChildNodes(*this);

    if(strcmp(p_if.getCond()->getType(), "boolean") != 0 && strcmp(p_if.getCond()->getType(), "error-type") != 0){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_if, CONDITION_NOT_BOOL, p_if.getCond()->getLocation().line-1, "cond");
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_while.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    char* final_value_type = strdup(p_for.getExpr()->getType());
    char* init_value_type = strdup(p_for.getAssignment()->getExpr()->getType());
    if(strcmp(final_value_type, "integer") == 0 && strcmp(init_value_type, "integer") == 0){
        if(p_for.getExpr()->getValue().int_value < p_for.getAssignment()->getExpr()->getValue().int_value){
            this->has_error = true;
            this->err_man.addErrorMsg(&p_for, LOOP_VAR_DEC, p_for.getLocation().line-1, "for");
        }
    }

    SymbolTable* symtab = new SymbolTable();
    this->sym_man.pushScope(symtab);
    p_for.visitChildNodes(*this);
    this->sym_man.popScope();

}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_return.visitChildNodes(*this);

    this->sym_man.setReturnType(p_return);
    // printf("return type: %s\n", p_return.getReturnType());

    // printf("return: %s, actual ret: %s\n", p_return.getReturnType(), p_return.getExpr()->getType());
    if(strcmp(p_return.getReturnType(), p_return.getExpr()->getType()) != 0 && strcmp(p_return.getExpr()->getType(), "error-type") == 0){
        return;
    }else if(strcmp(p_return.getReturnType(), "program-error") == 0){

        this->has_error = true;
        this->err_man.addErrorMsg(&p_return, RET_POS_ERROR, p_return.getLocation().line-1, "program-return");
        return;
    }else if(strcmp(p_return.getReturnType(), p_return.getExpr()->getType()) != 0 && strcmp(p_return.getExpr()->getType(), "error-type") != 0){
        this->has_error = true;
        this->err_man.addErrorMsg(&p_return, RET_TYPE_ERROR, p_return.getLocation().line-1, "return");
        return;
    }

    // this->sym_man.popFunctStack();
}

bool SemanticAnalyzer::hasError(){
    return this->has_error;
}

void SemanticAnalyzer::showError(){
    this->err_man.showErrList();
}