#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 /* TODO: variable name, type, constant value */
                 const char *name, 
                 Variables *type,
                 AstNode *cons);
    ~VariableNode() = default;

    const char *getNameCString();
    const char *getNameCType();

    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

    void print() override;

  private:
    // TODO: variable name, type, constant value
    const char *name;
    Variables *type;
    AstNode *cons;
};

#endif
