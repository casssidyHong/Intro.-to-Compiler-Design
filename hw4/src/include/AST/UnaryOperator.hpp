#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/AstDumper.hpp"

class UnaryOperatorNode : public ExpressionNode {
    private:
    // TODO: operator, expression
        std::string op;
        ExpressionNode* expr = nullptr;

    public:
        UnaryOperatorNode(const uint32_t line, const uint32_t col, const char* op, ExpressionNode* expr
                        /* TODO: operator, expression */);
        ~UnaryOperatorNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void setType(const char* type) override;
        const char* getType() override;
        void setValue(ConstValue value) override;
        ConstValue getValue() override;

        ExpressionNode* getExpr();

        const char* getOp();
};

#endif
