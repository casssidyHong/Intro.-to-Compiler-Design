#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"

class ReadNode : public AstNode {
    private:
        // TODO: variable reference
        VariableReferenceNode* var_ref = nullptr;
        AstNode* next_stmt = nullptr;

    public:
        ReadNode(const uint32_t line, const uint32_t col, VariableReferenceNode* var_ref
                /* TODO: variable reference */);
        ~ReadNode() = default;

        void print() override;
        void visitChildNodes(AstNodeVisitor& p_visitor);
        void accept(AstNodeVisitor& p_visitor);
        void setNextNode(AstNode* next_node);
        AstNode* getNextNode();

        void setVarRef(VariableReferenceNode* var_ref);
        VariableReferenceNode* getVarRef();
};

#endif
