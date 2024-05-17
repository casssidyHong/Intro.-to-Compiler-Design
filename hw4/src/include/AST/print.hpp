#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include <vector>
class PrintNode : public AstNode {
    private:
        // TODO: expression
        ExpressionNode* expr;
        AstNode* next_stmt = nullptr;

    public:
        PrintNode(const uint32_t line, const uint32_t col, ExpressionNode* expr
              /* TODO: expression */);
        ~PrintNode() = default;

        void print() override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;

        void setExpr(ExpressionNode* expr);
        ExpressionNode* getExpr();
};

#endif
