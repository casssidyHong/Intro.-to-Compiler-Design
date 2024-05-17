#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class IfNode : public AstNode {
    private:
        // TODO: expression, compound statement, compound statement
        AstNode* next_stmt = nullptr;
        ExpressionNode* cond = nullptr;
        CompoundStatementNode* body_stmt = nullptr;
        CompoundStatementNode* opt_stmt = nullptr;

    public:
        IfNode(const uint32_t line, const uint32_t col, ExpressionNode* cond, CompoundStatementNode* body_stmt, CompoundStatementNode* opt_stmt
            /* TODO: expression, compound statement, compound statement */);
        ~IfNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void setCond(ExpressionNode* cond);
        ExpressionNode* getCond();
};

#endif
