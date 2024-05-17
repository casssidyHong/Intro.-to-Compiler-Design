#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
               AstNode* expr, 
               AstNode* comp_if, 
               AstNode* comp_else)
    : AstNode{line, col}, expr(expr), 
      comp_if(comp_if), comp_else(comp_else) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr != NULL)
        expr->accept(p_visitor);
    if(comp_if != NULL)
        comp_if->accept(p_visitor);
    if(comp_else != NULL)
        comp_else->accept(p_visitor);
}