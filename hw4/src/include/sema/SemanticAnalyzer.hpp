#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolManager.hpp"
#include "sema/ErrorMessageManager.hpp"
// #include "visitor/AstNodeInclude.hpp"
// #include <vector>
// #include <string>
// #include <cstring>


class SemanticAnalyzer final : public AstNodeVisitor {
    private:
        // TODO: something like symbol manager (manage symbol tables)
        //       context manager, return type manager
        SymbolManager sym_man;
        ErrorManager err_man;
        bool has_error = false;

    public:
        ~SemanticAnalyzer() = default;
        SemanticAnalyzer(std::vector<char*> code);

        void visit(ProgramNode &p_program) override;
        void visit(DeclNode &p_decl) override;
        void visit(VariableNode &p_variable) override;
        void visit(ConstantValueNode &p_constant_value) override;
        void visit(FunctionNode &p_function) override;
        void visit(CompoundStatementNode &p_compound_statement) override;
        void visit(PrintNode &p_print) override;
        void visit(BinaryOperatorNode &p_bin_op) override;
        void visit(UnaryOperatorNode &p_un_op) override;
        void visit(FunctionInvocationNode &p_func_invocation) override;
        void visit(VariableReferenceNode &p_variable_ref) override;
        void visit(AssignmentNode &p_assignment) override;
        void visit(ReadNode &p_read) override;
        void visit(IfNode &p_if) override;
        void visit(WhileNode &p_while) override;
        void visit(ForNode &p_for) override;
        void visit(ReturnNode &p_return) override;

        bool hasError();
        void showError();
};

#endif
