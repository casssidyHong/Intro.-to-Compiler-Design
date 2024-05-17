#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, DeclNode* decls, const char* return_type, const char* name,
    CompoundStatementNode* cmpd_stmt, bool isFunct): AstNode{line, col}, return_type(return_type), name(name) {
    
    this->setDecls(decls);
    this->setCompoundStmt(cmpd_stmt);
    this->cmpd_stmt->setFunct(isFunct);

    for(DeclNode* decl : this->decls){
        std::vector<VariableNode*> vars = decl->getVarsVec();
        for(VariableNode* var : vars){
            var->setParam(true);
        }
    }
}

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, DeclNode* decls, const char* return_type, const char* name)
    : AstNode{line, col}, return_type(return_type), name(name) {
    
    this->setDecls(decls);
    for(DeclNode* decl : this->decls){
        std::vector<VariableNode*> vars = decl->getVarsVec();
        for(VariableNode* var : vars){
            var->setParam(true);
        }
    }
}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {
    std::printf("function declaration <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                "TODO", "TODO");
}

AstNode* FunctionNode::getNextNode(){
    return this->next_funct;
}

void FunctionNode::setNextNode(AstNode* next_node){
    this->next_funct = dynamic_cast<FunctionNode*>(next_node);
}

void FunctionNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void FunctionNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    for(auto decl : this->decls){
        decl->accept(p_visitor);
    }

    if(this->cmpd_stmt != nullptr){
        this->cmpd_stmt->accept(p_visitor);
    }
}

void FunctionNode::setCompoundStmt(CompoundStatementNode* cmpd_stmt){

    // CompoundStatementNode* cmpd_stmt_ptr = cmpd_stmt;
    this->cmpd_stmt = cmpd_stmt;
    // while(cmpd_stmt_ptr != nullptr){
    //     this->cmpd_stmt.insert(this->cmpd_stmts.begin(), cmpd_stmt_ptr);
    //     cmpd_stmt_ptr = cmpd_stmt_ptr->getNextCmpdStmt();
    // }
}

void FunctionNode::setDecls(DeclNode* decls){

    DeclNode* decl_ptr = decls;

    while(decl_ptr != nullptr){
        this->decls.insert(this->decls.begin(), decl_ptr);
        AstNode* next_decl = decl_ptr->getNextNode();
        decl_ptr = dynamic_cast<DeclNode*>(next_decl);
    }
}

const char* FunctionNode::getFunctName(){
    return this->name.c_str();
}

const char* FunctionNode::getReturnType(){
    return this->return_type.c_str();
}

int FunctionNode::getDeclNum(){
    return (int)this->decls.size();
}

std::vector<DeclNode*>& FunctionNode::getDeclVec(){
    return this->decls;
}

const char* FunctionNode::getParamType(){

    int cnt = 0;
    char* proto_type = (char*)calloc(100, sizeof(char));

    for(DeclNode* decl : this->decls){
        for(VariableNode* var : decl->getVarsVec()){
            if(cnt != 0)
                strcat(proto_type, ", ");

            strcat(proto_type, var->getVarType());
            cnt++;
        }
    }
    
    return proto_type;
}