#ifndef __AST_EXPRESSION_NODE_H
#define __AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include <string>

class ExpressionNode : public AstNode {
    protected:
        std::string type;
        ExpressionNode* next_expr = nullptr;
        ConstValue value;
        
    // for carrying type of result of an expression
    // TODO: for next assignment

    public:
        ExpressionNode(const uint32_t line, const uint32_t col);
        ~ExpressionNode() = default;

        virtual const char* getType() = 0;
        virtual void setType(const char* type) = 0;

        virtual void setValue(ConstValue value) = 0;
        virtual ConstValue getValue() = 0;
};

#endif
