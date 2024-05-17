#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                        /* TODO: operator, expressions */
                        Operator *op, 
                        AstNode *left, 
                        AstNode *right);
    ~BinaryOperatorNode() = default;

    const char *getNameCType();

    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

    void print() override;

  private:
    // TODO: operator, expressions
    AstNode *left;
    AstNode *right;
    Operator *op;
};

#endif
