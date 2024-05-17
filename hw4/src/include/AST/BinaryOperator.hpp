#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/AstDumper.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
    private:
        // TODO: operator, expressions
        std::string op;
        ExpressionNode* front_expr;
        ExpressionNode* back_expr;

    public:
        BinaryOperatorNode(const uint32_t line, const uint32_t col, const char* op, 
                            ExpressionNode* front_expr, ExpressionNode* back_expr
                        /* TODO: operator, expressions */);
        ~BinaryOperatorNode() = default;

        void print() override;
        void accept(AstNodeVisitor& p_visitor) override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void setType(const char* type) override;
        const char* getType() override;

        void setValue(ConstValue value) override;
        ConstValue getValue() override;

        ExpressionNode* getFrontExpr();
        ExpressionNode* getBackExpr();

        const char* getOp();
        
};

#endif
