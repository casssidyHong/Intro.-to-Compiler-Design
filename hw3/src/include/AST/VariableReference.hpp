#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"

class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          /* TODO: name */
                          const char* name, 
                          std::vector<AstNode*> *expr);
    // array reference
    // VariableReferenceNode(const uint32_t line, const uint32_t col
    // 
    ~VariableReferenceNode() = default;

    const char *getNameCString();

    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

    void print() override;

  private:
    // TODO: variable name, expressions
    const char* name;
    std::vector<AstNode*> *expr;
};

#endif
