#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "AST/decl.hpp"
#include <string>

class CompoundStatementNode : public AstNode {
	private:
    	// TODO: declarations, statements
		AstNode* next_stmt;
		bool funct = false;
		std::vector<DeclNode*> decls;
		std::vector<AstNode*> stmts;

  	public:
	    // test 
		CompoundStatementNode(const uint32_t line, const uint32_t col
                          /* TODO: declarations, statements */);
    	CompoundStatementNode(const uint32_t line, const uint32_t col, DeclNode* decls, AstNode* stmts
                          /* TODO: declarations, statements */);
    	~CompoundStatementNode() = default;

    	void print() override;
		void visitChildNodes(AstNodeVisitor& p_visitor) override;
		void accept(AstNodeVisitor& p_visitor) override;

		AstNode* getNextNode() override;
		void setNextNode(AstNode* next_node) override;

		void setDecls(DeclNode* decls);
		void setStmts(AstNode* stmts);
		bool isFunct();
		void setFunct(bool isFunct);
};

#endif
