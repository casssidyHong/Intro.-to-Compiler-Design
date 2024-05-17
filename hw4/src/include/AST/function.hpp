#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include <string>
#include <cstring>
#include <vector>

class FunctionNode : public AstNode {
    private:
        // TODO: name, declarations, return type, compound statement
        FunctionNode* next_funct = nullptr;
        std::string name;
        std::string return_type;
        std::vector<DeclNode*> decls;
        CompoundStatementNode* cmpd_stmt = nullptr;

    public:
        FunctionNode(const uint32_t line, const uint32_t col, DeclNode* decls, const char* return_type, const char* name,
                    CompoundStatementNode* cmpd_stmt, bool isFunct);

        FunctionNode(const uint32_t line, const uint32_t col, DeclNode* decls, const char* return_type, const char* name);

        ~FunctionNode() = default;

        void print() override;
        AstNode* getNextNode() override;
        void setNextNode(AstNode* next_node) override;
        void visitChildNodes(AstNodeVisitor& p_visitor);
        void accept(AstNodeVisitor& p_visitor) override;

        void setCompoundStmt(CompoundStatementNode* cmpd_stmt);
        void setDecls(DeclNode* decls);

        const char* getFunctName();
        const char* getReturnType();
        int getDeclNum();
        std::vector<DeclNode*>& getDeclVec();

        const char* getParamType();
};

#endif
