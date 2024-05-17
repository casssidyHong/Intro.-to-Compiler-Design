#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, VariableReferenceNode* var_ref, ExpressionNode* expr)
    : AstNode{line, col} {
    
    this->var_ref = var_ref;
    this->expr = expr;
}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->var_ref->accept(p_visitor);
    this->expr->accept(p_visitor);
}

void AssignmentNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void AssignmentNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* AssignmentNode::getNextNode(){
    return this->next_stmt;
}

void AssignmentNode::setVarRef(VariableReferenceNode* var_ref){
    this->var_ref = var_ref;
}

VariableReferenceNode* AssignmentNode::getVarRef(){
    return this->var_ref;
}

void AssignmentNode::setExpr(ExpressionNode* expr){
    this->expr = expr;
}

ExpressionNode* AssignmentNode::getExpr(){
    return this->expr;
}