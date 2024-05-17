#include "AST/variable.hpp"

VariableNode::VariableNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col}{}

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, const char* name)
    : AstNode{line, col}, name(name){}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {

    std::printf("variable <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                "TODO", "TODO");

}

void VariableNode::setVarType(const char* type){
    this->type = type;
}

void VariableNode::assignValue(ConstantValueNode* value_node){
    this->value_node = value_node;
    this->assigned = true;
}

AstNode* VariableNode::getNextNode(){
    return this->next_var;
}

void VariableNode::setNextNode(AstNode* next_node){
    this->next_var = dynamic_cast<VariableNode*>(next_node);
}

void VariableNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void VariableNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    if(this->value_node != nullptr)
        this->value_node->accept(p_visitor);
}

const char* VariableNode::getVarName(){
    return this->name.c_str();
}

const char* VariableNode::getVarType(){
    return this->type.c_str();
}

bool VariableNode::isAssigned(){
    return this->assigned;
}

void VariableNode::setParam(bool isParam){
    this->param = isParam;
}

bool VariableNode::isParam(){
    return this->param;
}

void VariableNode::setLoopVar(bool isLoopVar){
    this->loop_var = isLoopVar;
}

bool VariableNode::isLoopVar(){
    return this->loop_var;
}

AstNode* VariableNode::getValueNode(){
    return this->value_node;
}