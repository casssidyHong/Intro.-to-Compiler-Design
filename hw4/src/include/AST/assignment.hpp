#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "AST/VariableReference.hpp"

class AssignmentNode : public AstNode {
    private:
        // TODO: variable reference, expression
        VariableReferenceNode* var_ref = nullptr;
        ExpressionNode* expr = nullptr;
        AstNode* next_stmt = nullptr; 

    public:
        AssignmentNode(const uint32_t line, const uint32_t col, VariableReferenceNode* var_ref, ExpressionNode* expr);
        
        ~AssignmentNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor) override;
        void accept(AstNodeVisitor& p_visitor) override;
        void setNextNode(AstNode* next_node) override;
        AstNode* getNextNode() override;

        void setVarRef(VariableReferenceNode* var_ref);
        VariableReferenceNode* getVarRef();

        void setExpr(ExpressionNode* expr);
        ExpressionNode* getExpr();
};

#endif
