#include "AST/assignment.hpp"

AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                                AstNode* var_ref,
                                AstNode* expr)
    : AstNode{line, col}, var_ref(var_ref), expr(expr) {}

void AssignmentNode::print() {}


void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(var_ref != NULL)
        var_ref->accept(p_visitor);

    if(expr != NULL)
        expr->accept(p_visitor);
}

void AssignmentNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}