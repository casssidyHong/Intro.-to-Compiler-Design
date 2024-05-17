#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"

class AssignmentNode : public AstNode {
  public:
    AssignmentNode( const uint32_t line, const uint32_t col, 
                    /* TODO: variable reference, expression */
                    AstNode* var_ref,
                    AstNode* expr);
    ~AssignmentNode() = default;

    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &visitor) override;

    void print() override;

  private:
      // TODO: variable reference, expression
      AstNode* var_ref;
      AstNode* expr;
};

#endif
