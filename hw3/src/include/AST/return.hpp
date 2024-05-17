#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"

class ReturnNode : public AstNode {
  public:
    ReturnNode(const uint32_t line, const uint32_t col,
               /* TODO: expression */
               AstNode* expr);
    ~ReturnNode() = default;

    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

    void print() override;

  private:
    // TODO: expression
    AstNode* expr;
};

#endif
