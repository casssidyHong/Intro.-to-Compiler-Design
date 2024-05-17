#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col, ExpressionNode* expr)
    : AstNode{line, col} {
    
    this->expr = expr;
}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

void ReturnNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->expr->accept(p_visitor);
}

void ReturnNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void ReturnNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* ReturnNode::getNextNode(){
    return this->next_stmt;
}

void ReturnNode::setExpr(ExpressionNode* expr){
    this->expr = expr;
}

ExpressionNode* ReturnNode::getExpr(){
    return this->expr;
}

void ReturnNode::setReturnType(const char* type){
    this->return_type = type;
}

const char* ReturnNode::getReturnType(){
    return this->return_type;
}