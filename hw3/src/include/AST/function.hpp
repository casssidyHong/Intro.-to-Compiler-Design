#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */
                 const char *name, 
                 std::vector <AstNode*> *decl, 
                 std::vector <Variables*> *para,
                 Variables *return_type, 
                 AstNode *comp);
    ~FunctionNode() = default;

    const char *getNameCString();
    const char *getNameCType();
    std::string getNameCParam();

    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

    void print() override;

  private:
    // TODO: name, declarations, return type, compound statement
    const char *name;
    std::vector <AstNode*> *decl;
    std::vector <Variables*> *para;
    Variables *return_type;
    AstNode *comp;
};

#endif
