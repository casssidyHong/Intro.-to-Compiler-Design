#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, 
                     AstNode* expr)
    : AstNode{line, col}, expr(expr) {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr != NULL){
        expr->accept(p_visitor);
    }
}
