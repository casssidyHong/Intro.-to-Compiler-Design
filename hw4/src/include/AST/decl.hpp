#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "AST/AstDumper.hpp"
#include <string>
#include <vector>

class DeclNode : public AstNode {
    private:
        DeclNode* next_decl = nullptr;
        std::vector<VariableNode*> vars;
        
    public:
        // test
        DeclNode(const uint32_t line, const uint32_t col, const char* type);

        // variable declaration
    	DeclNode(const uint32_t line, const uint32_t col, const char* type, VariableNode* id_list, bool isParam
             /* TODO: identifiers, type */);

        // constant variable declaration
        DeclNode(const uint32_t line, const uint32_t col, ConstantValueNode* value, VariableNode* id_list
                /* TODO: identifiers, constant */);

    	~DeclNode() = default;

    	void print() override;
        AstNode* getNextNode() override;
        void setNextNode(AstNode* next_node) override;
        void accept(AstNodeVisitor &p_visitor) override;
        void visitChildNodes(AstNodeVisitor &p_visitor) override;

        std::vector<VariableNode*>& getVarsVec();
};

#endif
