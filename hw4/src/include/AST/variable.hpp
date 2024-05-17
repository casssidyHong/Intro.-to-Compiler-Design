#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/AstDumper.hpp"
#include <string>

class VariableNode : public AstNode {
	private:
		std::string name;
		std::string type;
		bool assigned = false;
		bool param = false;
		bool loop_var = false;
		ConstantValueNode* value_node = nullptr;
		VariableNode* next_var = nullptr;

  	public:
    	VariableNode(const uint32_t line, const uint32_t col, const char* name);
		VariableNode(const uint32_t line, const uint32_t col);

    	~VariableNode() = default;

    	void print() override;

		void setVarType(const char* type);
		void assignValue(ConstantValueNode* value_node);
		AstNode* getNextNode() override;
		void setNextNode(AstNode* next_node) override;
		void accept(AstNodeVisitor& p_visitor) override;
		void visitChildNodes(AstNodeVisitor& p_visitor) override;

		const char* getVarName();
		const char* getVarType();
		bool isAssigned();

		void setParam(bool isParam);
		bool isParam();

		void setLoopVar(bool isLoopVar);
		bool isLoopVar();
		
		AstNode* getValueNode();
};

#endif
