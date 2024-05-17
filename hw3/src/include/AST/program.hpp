#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>

class ProgramNode final : public AstNode {
  private:
    const std::string name;
    // TODO: return type, declarations, functions, compound statement
    const std::string return_type;
    std::vector <AstNode*> *decl;
    std::vector <AstNode*> *func;
    AstNode *body;

  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *name, 
                /* TODO: return type, declarations, functions,
                *       compound statement */
                const char *return_type,
                std::vector <AstNode*> *decl,
                std::vector <AstNode*> *func,
                AstNode *body
                );
    
    const char *getNameCString() const { return name.c_str(); }

    void accept(AstNodeVisitor &visitor) override { visitor.visit(*this); };
    void visitChildNodes(AstNodeVisitor &p_visitor);

    // visitor pattern version: const char *getNameCString() const;
    void print() override;
};

#endif
