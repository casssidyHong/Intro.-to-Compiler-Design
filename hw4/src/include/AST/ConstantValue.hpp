#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include <string>

class ConstantValueNode : public ExpressionNode {
    private:
    // TODO: constant value

    public:
        ConstantValueNode(const uint32_t line, const uint32_t col, const char* type, ConstValue value);
        ~ConstantValueNode() = default;

        void print() override;
        const char* getType() override; 
        void setType(const char* type) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void setValue(ConstValue value) override;
        ConstValue getValue() override;
};

#endif
