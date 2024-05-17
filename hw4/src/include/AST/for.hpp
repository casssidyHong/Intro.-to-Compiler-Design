#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/assignment.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class ForNode : public AstNode {
    private:
        // TODO: declaration, assignment, expression, compound statement
        DeclNode* decl = nullptr;
        AssignmentNode* assignment = nullptr;
        ExpressionNode* expr = nullptr;
        CompoundStatementNode* body_stmt = nullptr;
        AstNode* next_stmt = nullptr;

    public:
        ForNode(const uint32_t line, const uint32_t col, DeclNode* decl, AssignmentNode* assignment, 
                ExpressionNode* expr, CompoundStatementNode* body_stmt
                /* TODO: declaration, assignment, expression,
                *       compound statement */);
        ~ForNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void setExpr(ExpressionNode* expr);
        ExpressionNode* getExpr();

        void setAssignment(AssignmentNode* assign_node);
        AssignmentNode* getAssignment();
};

#endif
