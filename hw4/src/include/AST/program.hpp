#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/AstDumper.hpp"

#include <string>
#include <vector>

class ProgramNode final : public AstNode {
    private:
        std::string name;
        std::string return_type = "void";

        std::vector<DeclNode*> decls;
        std::vector<FunctionNode*> functs;
        CompoundStatementNode* cmpd_stmt;
        
        // TODO: return type, declarations, functions, compound statement

    public:
        ~ProgramNode() = default;
        ProgramNode(const uint32_t line, const uint32_t col,
                const char *const p_name, const char* const return_type,
                DeclNode* decls, FunctionNode* functs, CompoundStatementNode* cmpd_stmt);
    
    // visitor pattern version: const char *getNameCString() const;
        void print() override;

        void setDecls(DeclNode* decls);
        void setFuncts(FunctionNode* functs);
        void setCompoundStmt(CompoundStatementNode* cmpd_stmt);

        void visitChildNodes(AstNodeVisitor &p_visitor) override;
        const char* getProgName();
        void accept(AstNodeVisitor& p_visitor) override;
        AstNode* getNextNode() override;
        void setNextNode(AstNode* next_node) override;
};

#endif
