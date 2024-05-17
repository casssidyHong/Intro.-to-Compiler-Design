#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector <AstNode*> *decl,
                                             std::vector <AstNode*> *stat)
    : AstNode{line, col}, decl(decl), stat(stat) {}
// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(decl != NULL){
        for (auto &decl : *decl) 
            decl->accept(p_visitor);
    }
    if(stat != NULL){
        for (auto &stat : *stat)
            stat->accept(p_visitor);
    }
}
