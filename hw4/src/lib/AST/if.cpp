#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, ExpressionNode* cond, CompoundStatementNode* body_stmt, CompoundStatementNode* opt_stmt)
    : AstNode{line, col} {
    
    this->cond = cond;
    this->body_stmt = body_stmt;
    this->opt_stmt = opt_stmt;
}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->cond->accept(p_visitor);
    
    this->body_stmt->accept(p_visitor);

    if(this->opt_stmt){
        this->opt_stmt->accept(p_visitor);
    }
}

void IfNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void IfNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* IfNode::getNextNode(){
    return this->next_stmt;
}

void IfNode::setCond(ExpressionNode* cond){
    this->cond = cond;
}

ExpressionNode* IfNode::getCond(){
    return this->cond;
}