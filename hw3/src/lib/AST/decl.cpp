#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector <AstNode*> *vari)
    : AstNode{line, col}, vari(vari) {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(vari != NULL){
        for (auto &vari : *vari)
            vari->accept(p_visitor);
    }
}