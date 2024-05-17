#ifndef CODEGEN_CODE_GENERATOR_H
#define CODEGEN_CODE_GENERATOR_H

#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <map>

class CodeGenerator final : public AstNodeVisitor {
  private:
    const SymbolManager *m_symbol_manager_ptr;
    std::string m_source_file_path;
    // std::unique_ptr<FILE> m_output_file;
    std::unique_ptr<FILE, decltype(&fclose)> m_output_file{nullptr, &fclose};
    std::stack<int> mem_used;
    std::map<std::string, int> mem_pos_map;
    int cur_level = 0;
    int cur_assign_node = 0;
    int cur_if = 0;
    int cur_while = 0;
    int cur_for = 0;
    int for_end_cond = 0;
    std::stack<int> loop_var_found;
    std::stack<std::string> loop_var;
    std::stack<int> label_true;
    std::stack<int> label_false;
    int loop_num = 0;
    int label = 1;
    int cond_body = 0;
    int bool_assign = 0;
    int cur_real = 0;
    int cur_str = 0;
    int cur_arr = 0;
    int cur_funct_invoc = 0;
    int cur_funct = 0;
    int cur_read = 0;
    std::vector<int> arr_dims;
    std::map<std::string, int> real_map;
    std::vector<std::string> real_vec;
    std::map<std::string, int> str_map;
    std::vector<std::string> str_vec;

  public:
    ~CodeGenerator() = default;
    CodeGenerator(const std::string &source_file_name,
              const std::string &save_path,
              const SymbolManager *const p_symbol_manager);

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
};

#endif
