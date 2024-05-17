#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line, const uint32_t col, const char* name, ExpressionNode* exprs)
    : ExpressionNode{line, col}, name(name) {
    
    this->setExprs(exprs);
}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}

const char* FunctionInvocationNode::getFunctName(){
    return this->name.c_str();
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    for(ExpressionNode* expr : this->exprs){
        expr->accept(p_visitor);
    }
}

void FunctionInvocationNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void FunctionInvocationNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* FunctionInvocationNode::getNextNode(){
    return this->next_stmt;
}

void FunctionInvocationNode::setType(const char* type){
    this->type = type;
}

const char* FunctionInvocationNode::getType(){
    return this->type.c_str();
}

void FunctionInvocationNode::setValue(ConstValue value){
    this->value = value;
}

ConstValue FunctionInvocationNode::getValue(){
    return this->value;
}

void FunctionInvocationNode::setExprs(ExpressionNode* exprs){
    ExpressionNode* expr_ptr = exprs;
    while(expr_ptr != nullptr){
        this->exprs.insert(this->exprs.begin(), expr_ptr);
        expr_ptr = dynamic_cast<ExpressionNode*>(expr_ptr->getNextNode());
    }
}

std::vector<ExpressionNode*>& FunctionInvocationNode::getExprs(){
    return this->exprs;
}

void FunctionInvocationNode::setParamType(const char* param_type){
    this->param_type = param_type;
}

const char* FunctionInvocationNode::getParamType(){
    return this->param_type;
}