#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, const char* op, 
    ExpressionNode* front_expr, ExpressionNode* back_expr): ExpressionNode{line, col}, op(op) {
    
    this->front_expr = front_expr;
    this->back_expr = back_expr;
}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    this->front_expr->accept(p_visitor);
    this->back_expr->accept(p_visitor);
}

void BinaryOperatorNode::setNextNode(AstNode* next_node){
    this->next_expr = dynamic_cast<ExpressionNode*>(next_node);
}

AstNode* BinaryOperatorNode::getNextNode(){
    return this->next_expr;
}

const char* BinaryOperatorNode::getType(){
    return this->type.c_str();
}

void BinaryOperatorNode::setType(const char* type){
    this->type = type;
}

void BinaryOperatorNode::setValue(ConstValue value){
    this->value = value;
}

ConstValue BinaryOperatorNode::getValue(){
    return this->value;
}

ExpressionNode* BinaryOperatorNode::getFrontExpr(){
    return this->front_expr;
}

ExpressionNode* BinaryOperatorNode::getBackExpr(){
    return this->back_expr;
}

const char* BinaryOperatorNode::getOp(){
    return this->op.c_str();
}
