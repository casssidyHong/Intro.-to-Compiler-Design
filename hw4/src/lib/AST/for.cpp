#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col, DeclNode* decl, AssignmentNode* assignment, 
                ExpressionNode* expr, CompoundStatementNode* body_stmt): AstNode{line, col} {

    this->decl = decl;
    this->assignment = assignment;
    this->expr = expr;
    this->body_stmt = body_stmt;

}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor& p_visitor) {
    // TODO
    this->decl->accept(p_visitor);
    this->assignment->accept(p_visitor);
    this->expr->accept(p_visitor);
    this->body_stmt->accept(p_visitor);
}

void ForNode::accept(AstNodeVisitor& p_visitor){
    p_visitor.visit(*this);
}

void ForNode::setNextNode(AstNode* next_node){
    this->next_stmt = next_node;
}

AstNode* ForNode::getNextNode(){
    return this->next_stmt;
}

void ForNode::setExpr(ExpressionNode* expr){
    this->expr = expr;
}

ExpressionNode* ForNode::getExpr(){
    return this->expr;
}

void ForNode::setAssignment(AssignmentNode* assign_node){
    this->assignment = assignment;
}

AssignmentNode* ForNode::getAssignment(){
    return this->assignment;
}