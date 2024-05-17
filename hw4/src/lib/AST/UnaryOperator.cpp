#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, const char* op, ExpressionNode* expr)
    : ExpressionNode{line, col}, op(op) {
    
    this->expr = expr;
    this->setType(this->expr->getType());
}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    this->expr->accept(p_visitor);
}

void UnaryOperatorNode::setNextNode(AstNode* next_node){
    this->next_expr = dynamic_cast<ExpressionNode*>(next_node);
}

AstNode* UnaryOperatorNode::getNextNode(){
    return this->next_expr;
}

void UnaryOperatorNode::setType(const char* type){
    this->type = type;
}

const char* UnaryOperatorNode::getType(){
    return this->type.c_str();
}

void UnaryOperatorNode::setValue(ConstValue value){
    this->value = value;
}

ConstValue UnaryOperatorNode::getValue(){
    return this->value;
}

ExpressionNode* UnaryOperatorNode::getExpr(){
    return this->expr;
}

const char* UnaryOperatorNode::getOp(){
    return this->op.c_str();
}

void UnaryOperatorNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}