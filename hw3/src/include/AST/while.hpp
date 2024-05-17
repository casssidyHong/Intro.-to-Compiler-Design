#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              /* TODO: expression, compound statement */
              AstNode* expr, 
              AstNode* comp);
    ~WhileNode() = default;

    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };

    void print() override;

  private:
    // TODO: expression, compound statement
    AstNode* expr;
    AstNode* comp;
};

#endif
