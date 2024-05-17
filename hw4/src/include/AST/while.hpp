#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class WhileNode : public AstNode {
    private:
        // TODO: expression, compound statement
        ExpressionNode* cond = nullptr;
        CompoundStatementNode* body_stmt = nullptr;
        AstNode* next_stmt = nullptr;

    public:
        WhileNode(const uint32_t line, const uint32_t col, ExpressionNode* cond, CompoundStatementNode* body_stmt
                /* TODO: expression, compound statement */);
        ~WhileNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;
    
};

#endif
