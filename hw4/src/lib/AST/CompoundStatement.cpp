#include "AST/CompoundStatement.hpp"

// test
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {
}

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, DeclNode* decls, AstNode* stmts)
    : AstNode{line, col}{

    this->setDecls(decls);
    this->setStmts(stmts);
}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {
    // std::printf("compound statement <line: %u, col: %u>\n",
    //             location.line,
    //             location.col);
}

void CompoundStatementNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    for(DeclNode* decl : this->decls){
        decl->accept(p_visitor);
    }

    for(AstNode* stmt : this->stmts){
        // printf("Here!!\n");
        stmt->accept(p_visitor);
    }
}

void CompoundStatementNode::setDecls(DeclNode* decls){

    DeclNode* decl_ptr = decls;

    while(decl_ptr != nullptr){
        this->decls.insert(this->decls.begin(), decl_ptr);
        AstNode* next_decl = decl_ptr->getNextNode();
        decl_ptr = dynamic_cast<DeclNode*>(next_decl);
    }
}

void CompoundStatementNode::setStmts(AstNode* stmts){
    AstNode* stmt_ptr = stmts;

    while(stmt_ptr != nullptr){
        this->stmts.insert(this->stmts.begin(), stmt_ptr);
        stmt_ptr = stmt_ptr->getNextNode();
    }

    // printf("stmts size: %d\n", this->stmts.size());
}

AstNode* CompoundStatementNode::getNextNode(){
    return this->next_stmt;
}

void CompoundStatementNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

bool CompoundStatementNode::isFunct(){
    return this->funct;
}

void CompoundStatementNode::setFunct(bool isFunct){
    this->funct = isFunct;
}