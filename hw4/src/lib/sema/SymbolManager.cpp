#include "sema/SymbolManager.hpp"

SymbolEntry::SymbolEntry(const char* name, const char* kind, int level, 
                const char* type, AstNode* attr)
                : name(name), kind(kind), level(level), type(type){
    this->attr = attr;
}

// -------getting index------- //
const char* SymbolEntry::getName(){
    return name.c_str();
}

const char* SymbolEntry::getKind(){
    return kind.c_str();
}

int SymbolEntry::getLevel(){
    return level;
}

const char* SymbolEntry::getType(){
    return type.c_str();
}

AstNode* SymbolEntry::getAttr(){
    return attr;
}

// -------SYMBOL------- //
SymbolTable::SymbolTable(){}

void SymbolTable::addSymbol(const char* name, const char* kind, int level, const char* type, AstNode* attr){
    entries.push_back(SymbolEntry(name, kind, level, type, attr));
}

void SymbolTable::dumpSymbol() {
    for (size_t i=0; i<110; i++)
        printf("=");
    puts("");

    printf("%-33s%-11s%-11s%-17s%-11s\n", 
           "Name", "Kind", "Level", "Type", "Attribute");

    for (size_t i=0; i<110; i++)
        printf("-");
    puts("");
    
    for(SymbolEntry sym_ent : this->entries){
        printf("%-33s", sym_ent.getName());

        const char* kind = sym_ent.getKind();
        printf("%-11s", kind);

        if(sym_ent.getLevel() == 0)
            printf("%d%-10s", 0, "(global)");
        else
            printf("%d%-10s", sym_ent.getLevel(), "(local)");
        
        printf("%-17s", sym_ent.getType());
        
        if(strcmp(kind, "function") == 0){
            AstNode* attr = sym_ent.getAttr();
            FunctionNode* func = dynamic_cast<FunctionNode*>(attr); 
            printf("%-11s", func->getParamType());
        }else if(strcmp(kind, "constant") == 0){
            AstNode* attr = sym_ent.getAttr();
            ConstantValueNode* cons = dynamic_cast<ConstantValueNode*>(attr);
            const char* const_type = cons->getType();
            ConstValue value = cons->getValue();

            if(strcmp(const_type, "integer") == 0){
                printf("%-11d", value.int_value);
            }else if(strcmp(const_type, "real") == 0){
                printf("%-11f", value.real_value);
            }else if(strcmp(const_type, "boolean") == 0){
                if(value.bool_value)
                    printf("%-11s", "true");
                else printf("%-11s", "false");
            }else if(strcmp(const_type, "string") == 0){
                printf("%-11s", value.str_value);
            }
        }

        puts("");
    }
    
    for (size_t i = 0; i < 110; i++)
        printf("-");
    puts("");
}

SymbolTable* SymbolManager::getCurSymTab(){
    return this->st_tables.top();
}

std::vector<SymbolEntry>& SymbolTable::getEntries(){
    return this->entries;
}

SymbolManager::SymbolManager(){}

void SymbolManager::pushScope(SymbolTable *new_scope){
    this->st_tables.push(new_scope);
    this->vec_tables.push_back(new_scope);
    this->level += 1;
}

void SymbolManager::popScope(){
    SymbolTable* symtab = this->st_tables.top();
    this->vec_tables.pop_back();

    if(opt_sym)
        symtab->dumpSymbol();

    this->st_tables.pop();
    this->level -= 1;
}

int SymbolManager::getCurLevel(){
    return this->level;
}

const char* SymbolManager::getFunctInvocType(const char* name){
    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 && strcmp(entryIt->getKind(), "function") == 0) {
                AstNode* attr = entryIt->getAttr();
                FunctionNode* funct = dynamic_cast<FunctionNode*>(attr);
                return funct->getReturnType();
            }
            ++entryIt;
        }
        ++symtabIt;
    }
}

void SymbolManager::setVarRefType(VariableReferenceNode& p_var_ref){

    char* type;

    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), p_var_ref.getRefName()) == 0) {
                type = strdup(entryIt->getType());
                break;  // 找到目標後可以提前結束迴圈
            }
            ++entryIt;
        }
        ++symtabIt;
    }

    if(strcmp(p_var_ref.getType(), "array") != 0){
        p_var_ref.setType(type);
        return;
    }

    int dim_idx = p_var_ref.getExprs().size();
    std::vector<char*> dim;

    char* ref_type = (char*)calloc(50, sizeof(char));
    char* scalar_type = strtok(type, "[");
    char* dim_tok = strtok(NULL, "[");

    while(dim_tok != NULL){
        char* new_dim_tok = strdup(dim_tok);
        dim.push_back(new_dim_tok);
        dim_tok = strtok(NULL, "[");
    }

    strcpy(ref_type, scalar_type);

    if(dim_idx == dim.size()){
        ref_type[strlen(scalar_type)-1] = 0;
        p_var_ref.setType(ref_type);
        return;
    }

    for(int i = dim_idx; i < dim.size(); i++){
        strcat(ref_type, "[");
        strcat(ref_type, dim[i]);
    }

    p_var_ref.setType(ref_type);
    return;
}

const char* SymbolManager::getFunctParamType(const char* name){
    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0) {
                FunctionNode* funct = dynamic_cast<FunctionNode*>(entryIt->getAttr());
                return funct->getParamType();
            }
            ++entryIt;
        }
        ++symtabIt;
    }

    return NULL;
}

const char* SymbolManager::getFunctReturnType(const char* name){
    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0) {
                FunctionNode* funct = dynamic_cast<FunctionNode*>(entryIt->getAttr());
                return entryIt->getType();
            }
            ++entryIt;
        }
        ++symtabIt;
    }
    return NULL;
}

void SymbolManager::setReturnType(ReturnNode& return_node){

    if(this->funct_stack.empty()){
        return_node.setReturnType("program-error");
        return;
    }
    
    for(int i = this->vec_tables.size()-1; i >= 0; i--){
        std::vector<SymbolEntry> entries = this->vec_tables[i]->getEntries();
        for(int j = entries.size()-1; j >= 0; j--){
            if(strcmp(entries[j].getKind(), "function") == 0 && strcmp(this->funct_stack.top(), entries[j].getName()) == 0){
                return_node.setReturnType(strdup(entries[j].getType()));
                return;
            }
        }
    }
}

void SymbolManager::pushFunctStack(const char* name){
    this->funct_stack.push(name);
}

void SymbolManager::popFunctStack(){
    this->funct_stack.pop();
}

bool SymbolManager::isFunctStackEmpty(){
    return this->funct_stack.empty();
}

bool SymbolManager::isRedecl(const char* name){

    SymbolTable* cur_symtab = this->getCurSymTab();

    auto entryIt = cur_symtab->getEntries().begin();
    while (entryIt != cur_symtab->getEntries().end()) {
        if (strcmp(entryIt->getName(), name) == 0) {
            return true;
        }
        ++entryIt;
    }

    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 && strcmp(entryIt->getKind(), "loop_var") == 0) {
                return true;
            }
            ++entryIt;
        }
        ++symtabIt;
    }

    return false;

}

bool SymbolManager::isErrorDim(const char* type){

    if(strcmp(type, "integer") == 0 || strcmp(type, "real") == 0 || strcmp(type, "boolean") == 0 
                || strcmp(type, "string") == 0){
        
        return false;
    }

    char* arr_type = (char*)calloc(50, sizeof(char));
    strcpy(arr_type, type);

    char* tok = strtok(arr_type, "[");
    tok = strtok(NULL, "[");

    while(tok != NULL){
        char* ptr;
        long dim_val = strtol(tok, &ptr, 10);

        if(dim_val <= 0)
            return true;
        tok = strtok(NULL, "[");
    }

    return false;
}

bool SymbolManager::symNotFound(const char* name){
    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0) {
                return false;
            }
            ++entryIt;
        }
        ++symtabIt;
    }
    return true;
}

bool SymbolManager::symNotVar(const char* name){
    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 && 
                (strcmp(entryIt->getKind(), "variable") == 0 || 
                strcmp(entryIt->getKind(), "constant") == 0 || 
                strcmp(entryIt->getKind(), "loop_var") == 0)) {
                return false;
            }
            ++entryIt;
        }
        ++symtabIt;
    }
    return true;
}

bool SymbolManager::dimNotInt(VariableReferenceNode& var_ref){
    if(var_ref.exprNotInt())
        return true;
    else return false;
}

bool SymbolManager::overArrSubScript(VariableReferenceNode& var_ref){
    
    int dim_size = var_ref.getExprs().size();

    char* sym_type;

    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), var_ref.getRefName()) == 0 && strcmp(entryIt->getKind(), "variable") == 0) {
                sym_type = strdup(entryIt->getType());
                break;
            }
            ++entryIt;
        }
        ++symtabIt;
    }

    char* arr_type = strdup(sym_type);

    char* tok = strtok(arr_type, "[");
    tok = strtok(NULL, "[");

    int cnt = 0;

    while(tok != NULL){
        cnt++;
        tok = strtok(NULL, "[");
    }

    if(dim_size > cnt)
        return true;
    else return false;
}

bool SymbolManager::invalidBinOp(BinaryOperatorNode& bin_op){

    if(strcmp(bin_op.getBackExpr()->getType(), "error-type") == 0){
        return false;
    }else if(strcmp(bin_op.getFrontExpr()->getType(), bin_op.getBackExpr()->getType()) == 0 && strcmp("string", bin_op.getFrontExpr()->getType()) == 0){

        if(strcmp(bin_op.getOp(), "+") == 0)
            return false;

    }else if((strcmp(bin_op.getOp(), "and") == 0 || strcmp(bin_op.getOp(), "or") == 0) && 
            (strcmp(bin_op.getFrontExpr()->getType(), "boolean") != 0 || strcmp(bin_op.getFrontExpr()->getType(), "boolean") != 0)){

        return true;

    }else if(strcmp(bin_op.getOp(), "mod") == 0 && (strcmp(bin_op.getFrontExpr()->getType(), "integer") != 0 || 
                strcmp(bin_op.getBackExpr()->getType(), "integer") != 0)){
        
        return true;

    }else if(strcmp(bin_op.getFrontExpr()->getType(), bin_op.getBackExpr()->getType()) == 0){

        return false;

    }else{
        if(strcmp(bin_op.getOp(), "and") != 0 && strcmp(bin_op.getOp(), "mod") != 0 && strcmp(bin_op.getOp(), "or") != 0){

            if((strcmp(bin_op.getFrontExpr()->getType(), "integer") == 0 || strcmp(bin_op.getFrontExpr()->getType(), "real") == 0) && 
                (strcmp(bin_op.getBackExpr()->getType(), "integer") == 0 || strcmp(bin_op.getBackExpr()->getType(), "real") == 0)){
                
                return false;
            }
        }
    }
    return true;
}

bool SymbolManager::invalidUnOp(UnaryOperatorNode& un_op){

    if(strcmp(un_op.getExpr()->getType(), "error-type") == 0){
        return false;
    }else if(strcmp(un_op.getExpr()->getType(), "boolean") != 0 && strcmp(un_op.getOp(), "not") == 0){
        return true;
    }else if((strcmp(un_op.getExpr()->getType(), "integer") == 0 || strcmp(un_op.getExpr()->getType(), "real") == 0) && strcmp(un_op.getOp(), "neg") == 0){
        return false;
    }else if(strcmp(un_op.getOp(), "neg") == 0){
        return true;
    }
    
    return false;

}

bool SymbolManager::isFunctDecl(const char* name){

    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0) {
                return true;
            }
            ++entryIt;
        }
        ++symtabIt;
    }
    return false;

    return false;
}

bool SymbolManager::isSymFunct(const char* name){
    auto symtabIt = this->vec_tables.begin();
    while (symtabIt != this->vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 && strcmp(entryIt->getKind(), "function") == 0) {
                return true;
            }
            ++entryIt;
        }
        ++symtabIt;
    }
    return false;
}

bool SymbolManager::invalidArgNum(int param_num, FunctionInvocationNode& funct_invoc){

    char* sym_param_type = (char*)funct_invoc.getParamType();

    int sym_type_num = 0;
    char* tmp_sym_param_type = strdup(sym_param_type);
    char* sym_param = strtok(tmp_sym_param_type, ",");

    while(sym_param != NULL){
        sym_type_num++;
        sym_param = strtok(NULL, ",");
    }

    if(sym_type_num != param_num)
        return true;
    else return false;
}

bool SymbolManager::argTypeNotMatch(FunctionInvocationNode& funct_invoc){

    std::vector<ExpressionNode*> expr = funct_invoc.getExprs();
    std::vector<char*> type;

    auto exprIt = expr.begin();
    while (exprIt != expr.end()) {
        type.push_back(strdup((*exprIt)->getType()));
        exprIt++;
    }

    char* sym_param_type = (char*)funct_invoc.getParamType();

    std::vector<char*> sym_type_vec;

    char* tmp_sym_param_type = strdup(sym_param_type);
    char* sym_param = strtok(tmp_sym_param_type, ",");

    bool first = true;
    while(sym_param != NULL){
        char* new_sym_param;

        if(first){
            new_sym_param = strdup(sym_param);
            first = false;
        }
        else 
            new_sym_param = strdup(sym_param+1);
        
        sym_type_vec.push_back(new_sym_param);
        sym_param = strtok(NULL, ",");
    }

    for(int i = 0; i < type.size(); i++){
        if(strcmp(sym_type_vec[i], type[i]) != 0)
            return true;
    }

    return false;
}

bool SymbolManager::isNotScalar(const char* type){
    if(strcmp(type, "boolean") != 0 && strcmp(type, "real") != 0 &&
       strcmp(type, "integer") != 0 && strcmp(type, "string") != 0)
        return true;
    else return false;
}

bool SymbolManager::isLoopVarOrContant(const char* name){
    auto tableIt = this->vec_tables.begin();
    while (tableIt != this->vec_tables.end()) {
        auto entryIt = (*tableIt)->getEntries().begin();
        while (entryIt != (*tableIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 &&
                (strcmp(entryIt->getKind(), "loop_var") == 0 ||
                strcmp(entryIt->getKind(), "constant") == 0)) {
                return true;
            }
            ++entryIt;
        }
        ++tableIt;
    }
    return false;
}

bool SymbolManager::isArray(const char* name) {
    auto symtabIt = vec_tables.begin();
    while (symtabIt != vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 &&
                (strcmp(entryIt->getType(), "integer") != 0 &&
                 strcmp(entryIt->getType(), "real") != 0 &&
                 strcmp(entryIt->getType(), "string") != 0 &&
                 strcmp(entryIt->getType(), "boolean") != 0)) {
                return true;
            }
            entryIt++;
        }
        symtabIt++;
    }
    return false;
}


bool SymbolManager::isConst(const char* name) {
    auto symtabIt = vec_tables.begin();
    while (symtabIt != vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 && strcmp(entryIt->getKind(), "constant") == 0)
                return true;
            entryIt++;
        }
        symtabIt++;
    }
    return false;
}

bool SymbolManager::isLoopVar(const char* name) {
    auto symtabIt = vec_tables.begin();
    while (symtabIt != vec_tables.end()) {
        auto entryIt = (*symtabIt)->getEntries().begin();
        while (entryIt != (*symtabIt)->getEntries().end()) {
            if (strcmp(entryIt->getName(), name) == 0 && strcmp(entryIt->getKind(), "loop_var") == 0 &&
                getCurLevel() > entryIt->getLevel()) {
                return true;
            }
            ++entryIt;
        }
        ++symtabIt;
    }
    return false;
}

bool SymbolManager::checkAssignType(AssignmentNode* assign_node){
    char* vari = strdup(assign_node->getVarRef()->getType());
    char* expr = strdup(assign_node->getExpr()->getType());

    if(strcmp(vari, expr) == 0)
        return true;
    else if(strcmp(vari, "real") == 0 && strcmp(expr, "integer") == 0)
        return true;
    else return false;
}

bool SymbolManager::checkExprError(std::vector<ExpressionNode*>& expr){
    auto it = expr.begin();
    while (it != expr.end()) {
        if (strcmp((*it)->getType(), "error-type") == 0) {
            return true;
        }
        ++it;
    }
    return false;
}