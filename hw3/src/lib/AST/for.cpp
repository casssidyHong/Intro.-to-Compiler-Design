#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                 AstNode* decl, 
                 AstNode* assi,
                 AstNode* expr, 
                 AstNode* comp)
    : AstNode{line, col}, decl(decl), assi(assi),
      expr(expr), comp(comp) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(decl != NULL)
        decl->accept(p_visitor);
    if(assi != NULL)
        assi->accept(p_visitor);
    if(expr != NULL)
        expr->accept(p_visitor);
    if(comp != NULL)
        comp->accept(p_visitor);
}