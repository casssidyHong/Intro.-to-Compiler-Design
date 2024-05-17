#ifndef SYMTAB_H
#define SYMTAB_H

#include <vector>
#include <stack>
#include <string>
#include "visitor/AstNodeInclude.hpp"
extern uint32_t opt_sym;

class SymbolEntry {
    private:

        std::string name;
        std::string kind;
        int level;
        std::string type;
        
        AstNode* attr = nullptr;

    public:
        SymbolEntry(const char* name, const char* kind, int level, 
                    const char* type, AstNode* attr);

        const char* getName();
        const char* getKind();
        int getLevel();
        const char* getType();

        AstNode* getAttr();

};

class SymbolTable {
    private:
        std::vector<SymbolEntry> entries;

    public:
        SymbolTable();
        void addSymbol(const char* name, const char* kind, int level, const char* type, AstNode* attr);
        // other methods

        void dumpDemarcation(const char chr);
        void dumpSymbol();
        
        std::vector<SymbolEntry>& getEntries();
};

class SymbolManager {
    private:
        std::stack <SymbolTable *> st_tables;
        std::vector<SymbolTable*> vec_tables;
        int level = -1;
        std::stack<const char*> funct_stack;

    public:
        SymbolManager();
        void pushScope(SymbolTable *new_scope);
        void popScope();
        // other methods
        int getCurLevel();
        SymbolTable* getCurSymTab();

        const char* getFunctInvocType(const char* name);
        void setVarRefType(VariableReferenceNode& var_ref);
        const char* getFunctParamType(const char* name);
        const char* getFunctReturnType(const char* name);
        void setReturnType(ReturnNode& return_node);
        void pushFunctStack(const char* name);
        void popFunctStack();
        bool isFunctStackEmpty();

        bool isRedecl(const char* name);
        bool isErrorDim(const char* type);
        bool symNotFound(const char* name);
        bool symNotVar(const char* name);
        bool dimNotInt(VariableReferenceNode& var_ref);
        bool overArrSubScript(VariableReferenceNode& var_ref);
        bool invalidBinOp(BinaryOperatorNode& bin_op);
        bool invalidUnOp(UnaryOperatorNode& un_op);
        bool isFunctDecl(const char* name);
        bool isSymFunct(const char* name);
        bool invalidArgNum(int param_num, FunctionInvocationNode& funct_invoc);
        bool argTypeNotMatch(FunctionInvocationNode& funct_invoc);
        bool isNotScalar(const char* type);
        bool isLoopVarOrContant(const char* name);
        bool isArray(const char* name);
        bool isConst(const char* name);
        bool isLoopVar(const char* name);
        bool checkAssignType(AssignmentNode* assign_node);
        bool checkExprError(std::vector<ExpressionNode*>& exprs);
};

#endif