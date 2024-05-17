#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
             /* TODO: variable reference */
             AstNode* vari);
    ~ReadNode() = default;

    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

    void print() override;
  
  private:
    // TODO: variable reference
    AstNode* vari;
};

#endif
