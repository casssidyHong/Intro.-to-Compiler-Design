#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col, ExpressionNode* cond, CompoundStatementNode* body_stmt)
    : AstNode{line, col} {
    
    this->cond = cond;
    this->body_stmt = body_stmt;
}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->cond->accept(p_visitor);
    this->body_stmt->accept(p_visitor);
}

void WhileNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void WhileNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* WhileNode::getNextNode(){
    return this->next_stmt;
}