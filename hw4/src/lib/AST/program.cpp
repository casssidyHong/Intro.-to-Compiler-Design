#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name, const char* const return_type,
                         DeclNode* decls, FunctionNode* functs, CompoundStatementNode* cmpd_stmts)
    : AstNode{line, col}, name(p_name), return_type(return_type) {
        this->setDecls(decls);
        this->setFuncts(functs);
        this->setCompoundStmt(cmpd_stmts);
}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    // std::printf("program <line: %u, col: %u> %s %s\n",
    //             location.line, location.col,
    //             name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}

void ProgramNode::setDecls(DeclNode* decls){

    DeclNode* decl_ptr = decls;

    while(decl_ptr != nullptr){
        this->decls.insert(this->decls.begin(), decl_ptr);
        AstNode* next_decl = decl_ptr->getNextNode();
        decl_ptr = dynamic_cast<DeclNode*>(next_decl);
    }
}

void ProgramNode::setFuncts(FunctionNode* functs){

    FunctionNode* funct_ptr = functs;

    while(funct_ptr != nullptr){
        this->functs.insert(this->functs.begin(), funct_ptr);
        AstNode* next_funct = funct_ptr->getNextNode();
        funct_ptr = dynamic_cast<FunctionNode*>(next_funct);
    }
}

void ProgramNode::setCompoundStmt(CompoundStatementNode* cmpd_stmt){

    // CompoundStatementNode* cmpd_stmt_ptr = cmpd_stmt;
    this->cmpd_stmt = cmpd_stmt;
    // while(cmpd_stmt_ptr != nullptr){
    //     this->cmpd_stmt.insert(this->cmpd_stmts.begin(), cmpd_stmt_ptr);
    //     cmpd_stmt_ptr = cmpd_stmt_ptr->getNextCmpdStmt();
    // }
}


void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version

    for(auto decl : this->decls){
        decl->accept(p_visitor);
    }

    for(auto funct : this->functs){
        funct->accept(p_visitor);
    }

    this->cmpd_stmt->accept(p_visitor);
}

const char* ProgramNode::getProgName(){
    return this->name.c_str();
}

void ProgramNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

AstNode* ProgramNode::getNextNode(){}

void ProgramNode::setNextNode(AstNode* next_node){}
