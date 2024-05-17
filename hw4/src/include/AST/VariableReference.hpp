#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include <vector>
#include <cstring>

class VariableReferenceNode : public ExpressionNode {
    private:
        // TODO: variable name, expressions
        std::string name;
        std::vector<ExpressionNode*> exprs;
        ExpressionNode* next_expr = nullptr;

    public:

        // array reference
        VariableReferenceNode(const uint32_t line, const uint32_t col, const char* name, ExpressionNode* exprs);
        
        // normal reference
        VariableReferenceNode(const uint32_t line, const uint32_t col, const char* name);
        
        ~VariableReferenceNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;
        void setType(const char* type) override;
        const char* getType() override;
        void setValue(ConstValue value) override;
        ConstValue getValue() override;

        void setExprs(ExpressionNode* exprs);
        std::vector<ExpressionNode*>& getExprs();
        const char* getRefName();

        bool exprNotInt();
};

#endif
