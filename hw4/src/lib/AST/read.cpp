#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, VariableReferenceNode* var_ref)
    : AstNode{line, col} {

    this->var_ref = var_ref;
}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

void ReadNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->var_ref->accept(p_visitor);
}

void ReadNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void ReadNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* ReadNode::getNextNode(){
    return this->next_stmt;
}

void ReadNode::setVarRef(VariableReferenceNode* var_ref){
    this->var_ref = var_ref;
}

VariableReferenceNode* ReadNode::getVarRef(){
    return this->var_ref;
}