#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col, const char* name)
    : ExpressionNode{line, col}, name(name) {

    this->setType("not-array");
}

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col, const char* name, ExpressionNode* exprs)
    : ExpressionNode{line, col}, name(name) {
    
    this->setExprs(exprs);
    if(this->exprs.size() > 0)
        this->setType("array");
}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    for(ExpressionNode* expr : this->exprs){
        expr->accept(p_visitor);
    }
}

void VariableReferenceNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void VariableReferenceNode::setNextNode(AstNode* next_node){
    this->next_expr = dynamic_cast<ExpressionNode*>(next_node);
}

AstNode* VariableReferenceNode::getNextNode(){
    return this->next_expr;
}

void VariableReferenceNode::setType(const char* type){
    this->type = type;
}

const char* VariableReferenceNode::getType(){
    return this->type.c_str();
}

void VariableReferenceNode::setValue(ConstValue value){
    this->value = value;
}

ConstValue VariableReferenceNode::getValue(){
    return this->value;
}

void VariableReferenceNode::setExprs(ExpressionNode* exprs){
    ExpressionNode* expr_ptr = exprs;

    while(expr_ptr != nullptr){
        this->exprs.insert(this->exprs.begin(), expr_ptr);
        expr_ptr = dynamic_cast<ExpressionNode*>(expr_ptr->getNextNode());
    }
}

std::vector<ExpressionNode*>& VariableReferenceNode::getExprs(){
    return this->exprs;
}

const char* VariableReferenceNode::getRefName(){
    return this->name.c_str();
}

bool VariableReferenceNode::exprNotInt(){
    for(ExpressionNode* expr : this->exprs){
        if(strcmp(expr->getType(), "integer") != 0)
            return true;
    }

    return false;
}
