#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, ExpressionNode* expr)
    : AstNode{line, col} {
    
    this->expr = expr;
}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* PrintNode::getNextNode(){
    return this->next_stmt;
}

void PrintNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->expr->accept(p_visitor);
}

void PrintNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void PrintNode::setExpr(ExpressionNode* expr){
    this->expr = expr;
}

ExpressionNode* PrintNode::getExpr(){
    return this->expr;
}