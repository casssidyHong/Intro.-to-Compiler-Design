#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, const char* type, ConstValue value)
    : ExpressionNode{line, col}{
    
    this->setType(type);
    this->setValue(value);
}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {

    if(this->type == "string")
        std::printf("constant <line: %u, col: %u> %s\n", location.line, location.col, "String");
    else if(this->type == "int")
        std::printf("constant <line: %u, col: %u> %s\n", location.line, location.col, "int");
    else if(this->type == "real")
        std::printf("constant <line: %u, col: %u> %s\n", location.line, location.col, "real");
    else if(this->type == "boolean")
        std::printf("constant <line: %u, col: %u> %s\n", location.line, location.col, "boolean");

    // else if(this->type == "array")

}

const char* ConstantValueNode::getType(){
    return this->type.c_str();
}

void ConstantValueNode::setType(const char* type){
    this->type = type;
}

void ConstantValueNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void ConstantValueNode::setValue(ConstValue value){
    this->value = value;
}

ConstValue ConstantValueNode::getValue(){
    return this->value;
}

void ConstantValueNode::visitChildNodes(AstNodeVisitor& p_visitor){}

AstNode* ConstantValueNode::getNextNode(){
    return this->next_expr;
}

void ConstantValueNode::setNextNode(AstNode* next_node){
    this->next_expr = dynamic_cast<ExpressionNode*>(next_node);
}