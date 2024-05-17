#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char* name, 
                                             std::vector<AstNode*> *expr)
    : ExpressionNode{line, col}, name(name), expr(expr) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

// TODO
void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(expr != NULL){
        for (auto &expr : *expr) {
            expr->accept(p_visitor);
        }
    }
}

const char *VariableReferenceNode::getNameCString(){
    return name;
}
