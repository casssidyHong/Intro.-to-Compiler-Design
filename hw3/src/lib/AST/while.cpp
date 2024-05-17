#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     AstNode* expr, AstNode* comp)
    : AstNode{line, col}, expr(expr), comp(comp) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr != NULL)
        expr->accept(p_visitor);
    if(comp != NULL)
        comp->accept(p_visitor);
}