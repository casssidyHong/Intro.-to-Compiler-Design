#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char* name, 
                                               std::vector<AstNode*> *expr)
    : ExpressionNode{line, col}, name(name), expr(expr) {}

// TODO: You may use code snippets in AstDumper.cpp
const char *FunctionInvocationNode::getNameCString() const {
    return name;
}

void FunctionInvocationNode::print() {}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr != NULL){
        for (auto &expr : *expr)
            expr->accept(p_visitor);
    }
}