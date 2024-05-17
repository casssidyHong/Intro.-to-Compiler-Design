#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             const SymbolManager *const p_symbol_manager)
    : m_symbol_manager_ptr(p_symbol_manager),
      m_source_file_path(source_file_name) {
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind("/");
    auto dot_pos = source_file_name.rfind(".");

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

// CodeGenerator::CodeGenerator(const std::string source_file_name,
//                              const std::string save_path,
//                              const SymbolManager *const p_symbol_manager)
//     : m_symbol_manager_ptr(p_symbol_manager),
//       m_source_file_path(source_file_name) {
//     // FIXME: assume that the source file is always xxxx.p
    
//     const std::string &real_path =
//         (save_path == "") ? std::string{"."} : save_path;
//     auto slash_pos = source_file_name.rfind("/");
//     auto dot_pos = source_file_name.rfind(".");

//     if (slash_pos != std::string::npos) {
//         ++slash_pos;
//     } else {
//         slash_pos = 0;
//     }
//     std::string output_file_path(
//         real_path + "/" +
//         source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S");
//     m_output_file.reset(fopen(output_file_path.c_str(), "w"));
//     assert(m_output_file.get() && "Failed to open output file");
// }

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {

    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char*const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_program.getSymbolTable());

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    constexpr const char*const riscv_assembly_main_function =
        ".section    .text\n"
        "    .align 2\n"
        "    .globl main\n"
        "    .type main, @function\n"
        "main:\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_main_function,
                     m_source_file_path.c_str());


    constexpr const char*const riscv_assembly_main_prologue = 
        "    addi sp, sp, -128\n"
        "    sw   ra, 124(sp)\n"
        "    sw   s0, 120(sp)\n"
        "    addi s0, sp, 128\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_main_prologue,
                     m_source_file_path.c_str());

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    constexpr const char*const riscv_assembly_main_epilogue = 
        "    lw   ra, 124(sp)\n"
        "    lw   s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr   ra\n"
        "    .size main, .-main\n";


    dumpInstructions(m_output_file.get(), riscv_assembly_main_epilogue);


    for(int i = 0; i < (int)real_vec.size(); i++){
        constexpr const char*const riscv_real_value =
            ".section    .rodata\n"
            "    .align 2\n"
            "rv%d:\n"
            "    .float %s\n";

        dumpInstructions(m_output_file.get(), riscv_real_value, i, real_vec[i].c_str());
    }

    for(int i = 0; i < (int)str_vec.size(); i++){
        constexpr const char*const riscv_str_value =
            ".section    .rodata\n"
            "    .align 2\n"
            "st%d:\n"
            "    .string \"%s\"\n";

        dumpInstructions(m_output_file.get(), riscv_str_value, i, str_vec[i].c_str());
    }
    
    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };

    for_each(p_decl.getVariables().begin(), p_decl.getVariables().end(),
             visit_ast_node);

    // if(!mem_pos_map.empty())
    //     std::printf("lv: %d, lc: %d\n", mem_pos_map.top()["lv"], mem_pos_map.top()["lc"]);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    const Constant* constant = p_variable.getConstantPtr();

    const char* var_name = p_variable.getNameCString();
    size_t var_level = m_symbol_manager_ptr->lookup(var_name)->getLevel();
    const char* var_type = p_variable.getTypeCString();
    const char* var_value = nullptr;
    bool not_array = p_variable.getTypePtr()->isScalar();

    if(constant != nullptr){
        var_value = constant->getConstantValueCString();
    }

    int size = 0;


    if(strcmp(var_type, "integer") == 0 || strcmp(var_type, "real") == 0)
        size = 4;
    else if(strcmp(var_type, "boolean") == 0)
        size = 1;
    // else if(strcmp(var_type, "string") == 0)
    //     size = strlen(var_value);

    /* string is bonus */

    if(constant == nullptr && var_level == 0){
        constexpr const char*const riscv_global_var = 
            "    .comm %s, %d, %d\n";
        
        dumpInstructions(m_output_file.get(), riscv_global_var, var_name, size, size);
    }else if(constant != nullptr && var_level == 0){

        if(strcmp(var_type, "real") == 0){
            constexpr const char*const riscv_global_const =
                ".section    .rodata\n"
                "    .align 2\n"
                "    .globl %s\n"
                "    .type %s, @object\n"
                "%s:\n"
                "    .float %s\n";

            dumpInstructions(m_output_file.get(), riscv_global_const, var_name, var_name, var_name, var_value);
        }else if(strcmp(var_type, "string") == 0){
            constexpr const char*const riscv_global_const =
                ".section    .rodata\n"
                "    .align 2\n"
                "    .globl %s\n"
                "    .type %s, @object\n"
                "%s:\n"
                "    .string \"%s\"\n";

            dumpInstructions(m_output_file.get(), riscv_global_const, var_name, var_name, var_name, var_value);
        }else{
            constexpr const char*const riscv_global_const =
                ".section    .rodata\n"
                "    .align 2\n"
                "    .globl %s\n"
                "    .type %s, @object\n"
                "%s:\n"
                "    .word %s\n";

            dumpInstructions(m_output_file.get(), riscv_global_const, var_name, var_name, var_name, var_value);
        }
        
            
    }else if(var_level > 0){
        int cur_mem_pos = mem_used.top();

        // std::map<const char*, int> tmp_map = mem_pos_map.top();
        // mem_pos_map.pop();

        // tmp_map[var_name] = cur_mem_pos;
        // mem_pos_map.push(tmp_map);
        std::string var_name_str = p_variable.getName();
        mem_pos_map[var_name_str] = cur_mem_pos;
        // std::printf("var_name: %s, type: %s\n", var_name, var_type);

        if(!not_array){
            const std::vector<uint64_t> dims = p_variable.getTypePtr()->getDimensions();
            int sz = 1;
            for(int i = 0; i < (int)dims.size(); i++){
                sz *= dims[i];
            }

            cur_mem_pos += sz;
        }else{
            cur_mem_pos += 4;
        }
        
        mem_used.pop();
        mem_used.push(cur_mem_pos);

        if(constant != nullptr){
            if(strcmp(var_type, "real") == 0){
                std::string var_value_str(var_value);
                real_vec.push_back(var_value_str);
                // printf("string: %s\n", real_vec.back().c_str());
                real_map[var_name_str] = (int)real_vec.size()-1;
                cur_real = real_map[var_name_str];
            }else if(strcmp(var_type, "string") == 0){
                std::string var_value_str(var_value);
                str_vec.push_back(var_value_str);
                // printf("string: %s\n", real_vec.back().c_str());
                str_map[var_name_str] = (int)str_vec.size()-1;
                cur_str = str_map[var_name_str];
            }else{
                constexpr const char*const assign_left =
                    "    addi t0, s0, -%d\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t0, 0(sp)\n";
            
                dumpInstructions(m_output_file.get(), assign_left, mem_pos_map[var_name_str]);
            }


            const char* value = constant->getConstantValueCString();

            if(strcmp(var_type, "real") == 0){
                constexpr const char*const assign_expr =
                    "    lui  t0, %%hi(rv%d)\n"
                    "    flw ft0, %%lo(rv%d)(t0)\n"
                    "    fsw ft0, -%d(s0)\n";

                dumpInstructions(m_output_file.get(), assign_expr, cur_real, cur_real, mem_pos_map[var_name]);
            }else if(strcmp(var_type, "string") == 0){
                constexpr const char*const assign_expr =
                    "    lui  t0, %%hi(rv%d)\n"
                    "    addi t0, t0, %%lo(st%d)\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t0, 0(sp)\n";

                dumpInstructions(m_output_file.get(), assign_expr, cur_real, cur_real, mem_pos_map[var_name]);
                
            }else{
                constexpr const char*const assign_expr =
                    "    li   t0, %s\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t0, 0(sp)\n"
                    "    lw   t0, 0(sp)\n"
                    "    addi sp, sp, 4\n"
                    "    lw   t1, 0(sp)\n"
                    "    addi sp, sp, 4\n"
                    "    sw   t0, 0(t1)\n";

                dumpInstructions(m_output_file.get(), assign_expr, value);
            }

        }
    }
    
    // if(!mem_pos_map.empty())
    //     std::printf("var_name: %s, pos: %d\n", var_name, mem_pos_map.top()[var_name]);
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {

    // printf("constant\n");

    if(for_end_cond){

        constexpr const char*const riscv_label = 
            "L%d:\n";
    
        dumpInstructions(m_output_file.get(), riscv_label, label);
        label++;

        constexpr const char*const load_loop_var = 
            "    lw   t0, -%d(s0)\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        dumpInstructions(m_output_file.get(), load_loop_var, mem_pos_map[loop_var.top()]);
    }

    const char* value = p_constant_value.getConstantValueCString();
    bool is_bool = p_constant_value.getInferredType()->isBool();
    bool is_real = p_constant_value.getInferredType()->isReal();
    bool is_str = p_constant_value.getInferredType()->isString();
    // printf("is_bool: %d\n", is_bool);

    if(cur_arr){
        // printf("cur_dim: %s\n", value);
        int val = strtol(value, NULL, 10);
        // printf("cur_dim int: %d\n", val);
        arr_dims.push_back(val);
        return;
    }

    if(!is_real && !is_str){
        constexpr const char*const load_const =
            "    li   t0, %s\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        if(is_bool){
            if(strcmp(value, "true") == 0)
                dumpInstructions(m_output_file.get(), load_const, "1");
            else dumpInstructions(m_output_file.get(), load_const, "0");
        }else{
            dumpInstructions(m_output_file.get(), load_const, value);
        }
    }else if(is_str){
        const char* var_value = p_constant_value.getConstantValueCString();
        // printf("var value: %s\n", var_value);
        std::string var_value_str(var_value);
        // printf("str: %s\n", var_value_str.c_str());
        str_vec.push_back(var_value_str);
        // printf("string: %s\n", str_vec.back().c_str());

        constexpr const char*const load_const =
            "    lui  t0, %%hi(st%d)\n"
            "    addi t0, t0, %%lo(st%d)\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        dumpInstructions(m_output_file.get(), load_const, (int)str_vec.size()-1, (int)str_vec.size()-1);
    }else{

        const char* var_value = p_constant_value.getConstantValueCString();
        std::string var_value_str(var_value);
        real_vec.push_back(var_value_str);
        //printf("string: %s\n", real_vec.back().c_str());

        constexpr const char*const load_const =
            "    lui  t0, %%hi(rv%d)\n"
            "    flw ft0, %%lo(rv%d)(t0)\n"
            "    addi sp, sp, -4\n"
            "    fsw ft0, 0(sp)\n";

        dumpInstructions(m_output_file.get(), load_const, (int)real_vec.size()-1, (int)real_vec.size()-1);
    }
    
    

    if(for_end_cond){
        for_end_cond = 0;

        label_true.push(label);
        // printf("cur_label: %d\n", label);
        label++;
        label_false.push(label);
        // printf("cur_label: %d\n", label);
        label++;

        constexpr const char*const riscv_bin_op = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    lw   t1, 0(sp)\n"
            "    addi sp, sp, 4\n";

        dumpInstructions(m_output_file.get(), riscv_bin_op);

        constexpr const char*const comp =
            "    bge  t1, t0, L%d\n";

        dumpInstructions(m_output_file.get(), comp, label_false.top());
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_function.getSymbolTable());

    const char* funct_name = p_function.getNameCString();

    constexpr const char*const riscv_assembly_function =
        ".section    .text\n"
        "    .align 2\n"
        "    .globl %s\n"
        "    .type %s, @function\n"
        "%s:\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_function, funct_name, funct_name, funct_name);

    constexpr const char*const riscv_assembly_function_prologue = 
        "    addi sp, sp, -128\n"
        "    sw   ra, 124(sp)\n"
        "    sw   s0, 120(sp)\n"
        "    addi s0, sp, 128\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_function_prologue);

    mem_used.push(12);

    int param_num = p_function.getParametersNum(p_function.getParameters());

    int decl_idx = 0;
    int var_idx = 0;

    for(int i = 0; i < param_num; i++){
        int mem_pos = mem_used.top();
        bool is_real = p_function.getParameters()[decl_idx]->getVariables()[var_idx]->getTypePtr()->isReal();
    
        // bool is_str = p_function.getParameters()[decl_idx]->getVariables()[var_idx]->getTypePtr()->isString();
        // const char* value = p_function.getParameters()[decl_idx]->getVariables()[var_idx]->getConstantPtr()->getConstantValueCString();

        if(i < 8){

            if(is_real){
                constexpr const char*const riscv_push_arg = 
                    "    fsw fa%d, -%d(s0)\n";

                dumpInstructions(m_output_file.get(), riscv_push_arg, i, mem_pos);
            }else{
                constexpr const char*const riscv_push_arg =
                    "    sw   a%d, -%d(s0)\n";

                dumpInstructions(m_output_file.get(), riscv_push_arg, i, mem_pos);
            }
        
        }else{

            if(is_real){
                constexpr const char*const riscv_push_arg = 
                    "    fsw ft%d, -%d(s0)\n";

                dumpInstructions(m_output_file.get(), riscv_push_arg, i-6, mem_pos);
            }else{
                constexpr const char*const riscv_push_arg =
                    "    sw   t%d, -%d(s0)\n";

                dumpInstructions(m_output_file.get(), riscv_push_arg, i-6, mem_pos);
            }
            
        }
        
        std::string var_name = p_function.getParameters()[decl_idx]->getVariables()[var_idx]->getName();
        
        if(var_idx == (int)p_function.getParameters()[decl_idx]->getVariables().size() - 1){
            decl_idx++;
            var_idx = 0;
        }else var_idx++;

        mem_pos_map[var_name] = mem_pos;

        mem_used.pop();
        mem_used.push(mem_pos+4);
    }

    // printf("function\n");
    cur_funct = 1;

    p_function.visitBodyChildNodes(*this);

    cur_funct = 0;
    // printf("finish function\n");

    constexpr const char*const riscv_assembly_function_epilogue = 
        "    lw   ra, 124(sp)\n"
        "    lw   s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr   ra\n"
        "    .size %s, .-%s\n";

    dumpInstructions(m_output_file.get(), riscv_assembly_function_epilogue, funct_name, funct_name);

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_compound_statement.getSymbolTable());

    if(cur_if){
        if(!cond_body){
            constexpr const char*const riscv_label = 
                "L%d:\n";
        
            dumpInstructions(m_output_file.get(), riscv_label, label_true.top());
            label_true.pop();
            cond_body = 1;

        }else if(cond_body){
            constexpr const char*const riscv_label =
                "    j    L%d\n"
                "L%d:\n";

            dumpInstructions(m_output_file.get(), riscv_label, label, label_false.top());
            label_false.pop();

            label_true.push(label);

            label++;
            
        }
        
    }else if(cur_while || cur_for){
        constexpr const char*const riscv_label = 
            "L%d:\n";
    
        dumpInstructions(m_output_file.get(), riscv_label, label_true.top());
        label_true.pop();

    }else{
        mem_used.push(12);
    }

    p_compound_statement.visitChildNodes(*this);

    // mem_pos_map.pop();

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(
        p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);

    bool is_real = p_print.getTarget().getInferredType()->isReal();
    bool is_str = p_print.getTarget().getInferredType()->isString();

    if(is_real){
        constexpr const char*const riscv_print = 
            "    flw fa0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    jal  ra, printReal\n";

        dumpInstructions(m_output_file.get(), riscv_print);
    }else if(is_str){
        constexpr const char*const riscv_print = 
            "    lw   a0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    jal  ra, printString\n";

        dumpInstructions(m_output_file.get(), riscv_print);
    }else{
        constexpr const char*const riscv_print =
            "    lw   a0, 0(sp)\n"
            "    addi sp, sp, 4\n" 
            "    jal  ra, printInt\n";

        dumpInstructions(m_output_file.get(), riscv_print);
    }
    
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {

    if(cur_if)
        cur_if++;

    p_bin_op.visitChildNodes(*this);

    if(cur_if)
        cur_if--;
    
    bool is_real = p_bin_op.getInferredType()->isReal();

    if(is_real){
        constexpr const char*const riscv_bin_op = 
            "    flw  ft0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    flw  ft1, 0(sp)\n"
            "    addi sp, sp, 4\n";

        dumpInstructions(m_output_file.get(), riscv_bin_op); 
    }else{
        constexpr const char*const riscv_bin_op = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    lw   t1, 0(sp)\n"
            "    addi sp, sp, 4\n";

        dumpInstructions(m_output_file.get(), riscv_bin_op); 
    }
    
    
    if(p_bin_op.getOp() == Operator::kPlusOp){
        
        if(is_real){
            constexpr const char*const riscv_op = 
                "    fadd.s  ft0, ft1, ft0\n"
                "    addi sp, sp, -4\n"
                "    fsw ft0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }else{
            constexpr const char*const riscv_op = 
                "    add  t0, t1, t0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }
        
    }else if(p_bin_op.getOp() == Operator::kMultiplyOp){

        if(is_real){
            constexpr const char*const riscv_op = 
                "    fmul.s  ft0, ft1, ft0\n"
                "    addi sp, sp, -4\n"
                "    fsw ft0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }else{
            constexpr const char*const riscv_op = 
                "    mul  t0, t1, t0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }
        
    }else if(p_bin_op.getOp() == Operator::kDivideOp){

        if(is_real){
            constexpr const char*const riscv_op = 
                "    fdiv.s  ft0, ft1, ft0\n"
                "    addi sp, sp, -4\n"
                "    fsw ft0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }else{
            constexpr const char*const riscv_op = 
                "    div  t0, t1, t0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }
        
    }else if(p_bin_op.getOp() == Operator::kMinusOp){

        if(is_real){
            constexpr const char*const riscv_op = 
                "    fsub.s  ft0, ft1, ft0\n"
                "    addi sp, sp, -4\n"
                "    fsw ft0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }else{
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n";
            
            dumpInstructions(m_output_file.get(), riscv_op);
        }
        
    }else if(p_bin_op.getOp() == Operator::kModOp){

        constexpr const char*const riscv_op = 
            "    rem  t0, t1, t0\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";
        
        dumpInstructions(m_output_file.get(), riscv_op);
        
    }else if(p_bin_op.getOp() == Operator::kEqualOp){

        if(bool_assign || cur_if > 1){
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    bne  t0, zero, L%d\n"
                "    li   t0, 1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    j    L%d\n"
                "L%d:\n"
                "    li   t0, 0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "L%d:\n";
            
            dumpInstructions(m_output_file.get(), riscv_op, label, label+1, label, label+1);
        }else{
            if(is_real){
                constexpr const char*const riscv_op = 
                    "    bne  t1, t0, L%d\n";
                
                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }else{
                constexpr const char*const riscv_op = 
                    "    bne  t1, t0, L%d\n";
                
                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }
            
        }

    }else if(p_bin_op.getOp() == Operator::kGreaterOp){

        if(bool_assign || cur_if > 1){
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    ble  t0, zero, L%d\n"
                "    li   t0, 1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    j    L%d\n"
                "L%d:\n"
                "    li   t0, 0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "L%d:\n";
            
            dumpInstructions(m_output_file.get(), riscv_op, label, label+1, label, label+1);

            label += 2;
        }else{
            if(is_real){
                constexpr const char*const riscv_op = 
                    "    ble  t1, t0, L%d\n";

                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }else{
                constexpr const char*const riscv_op = 
                    "    ble  t1, t0, L%d\n";

                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }
            
        }
        

    }else if(p_bin_op.getOp() == Operator::kGreaterOrEqualOp){

        if(bool_assign || cur_if > 1){
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    blt  t0, zero, L%d\n"
                "    li   t0, 1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    j    L%d\n"
                "L%d:\n"
                "    li   t0, 0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "L%d:\n";
            
            dumpInstructions(m_output_file.get(), riscv_op, label, label+1, label, label+1);

            label += 2;
        }else{

            if(is_real){
                constexpr const char*const riscv_op = 
                    "    blt  t1, t0, L%d\n";

                    dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }else{
                constexpr const char*const riscv_op = 
                    "    blt  t1, t0, L%d\n";

                    dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }
            
        }
        

    }else if(p_bin_op.getOp() == Operator::kLessOp){

        if(bool_assign || cur_if > 1){
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    bge  t0, zero, L%d\n"
                "    li   t0, 1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    j    L%d\n"
                "L%d:\n"
                "    li   t0, 0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "L%d:\n";
            
            dumpInstructions(m_output_file.get(), riscv_op, label, label+1, label, label+1);

            label += 2;
        }else{

            if(is_real){
                constexpr const char*const riscv_op = 
                    "    bge  t1, t0, L%d\n";

                    dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }else{
                constexpr const char*const riscv_op = 
                    "    bge  t1, t0, L%d\n";

                    dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }
            
        }
        

    }else if(p_bin_op.getOp() == Operator::kLessOrEqualOp){

        if(bool_assign || cur_if > 1){
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    bgt  t0, zero, L%d\n"
                "    li   t0, 1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    j    L%d\n"
                "L%d:\n"
                "    li   t0, 0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "L%d:\n";
            
            dumpInstructions(m_output_file.get(), riscv_op, label, label+1, label, label+1);

            label += 2;
        }else{

            if(is_real){
                constexpr const char*const riscv_op = 
                    "    bgt  t1, t0, L%d\n";

                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }else{
                constexpr const char*const riscv_op = 
                    "    bgt  t1, t0, L%d\n";

                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }
            
        }
        

    }else if(p_bin_op.getOp() == Operator::kNotEqualOp){

        if(bool_assign || cur_if > 1){
            constexpr const char*const riscv_op = 
                "    sub  t0, t1, t0\n"
                "    beq  t0, zero, L%d\n"
                "    li   t0, 1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    j    L%d\n"
                "L%d:\n"
                "    li   t0, 0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "L%d:\n";
            
            dumpInstructions(m_output_file.get(), riscv_op, label, label+1, label, label+1);
        }else{

            if(is_real){
                constexpr const char*const riscv_op = 
                    "    beq  t1, t0, L%d\n";

                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }else{
                constexpr const char*const riscv_op = 
                    "    beq  t1, t0, L%d\n";

                dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
            }
            
        }
        
    }else if(p_bin_op.getOp() == Operator::kAndOp){

        constexpr const char*const riscv_op = 
            "    and  t0, t0, t1\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";
            
        dumpInstructions(m_output_file.get(), riscv_op);

        if(cur_if == 1){
            constexpr const char*const riscv_branch =
                "    lw   t0, 0(sp)\n"
                "    addi sp, sp, 4\n" 
                "    li   t1, 0\n"
                "    ble  t0, t1, L%d\n";

            dumpInstructions(m_output_file.get(), riscv_branch, label_false.top());
        }
        
    }else if(p_bin_op.getOp() == Operator::kOrOp){

        constexpr const char*const riscv_op = 
            "    or   t0, t0, t1\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";
            
        dumpInstructions(m_output_file.get(), riscv_op);
        
    }

}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {

    // printf("Unary\n");

    if(cur_if)
        cur_if++;

    p_un_op.visitChildNodes(*this);

    if(cur_if)
        cur_if--;

    constexpr const char*const load_value =
        "    lw   t0, 0(sp)\n"
        "    addi sp, sp, 4\n";

    dumpInstructions(m_output_file.get(), load_value);

    if(p_un_op.getOp() == Operator::kNotOp){

        constexpr const char*const riscv_op = 
            "    not  t0, t0\n"
            "    addi t0, t0, 2\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";
                
        dumpInstructions(m_output_file.get(), riscv_op);
        
        if(cur_if == 1){
            constexpr const char*const riscv_branch =
                "    lw   t0, 0(sp)\n"
                "    addi sp, sp, 4\n" 
                "    li   t1, 0\n"
                "    ble  t0, t1, L%d\n";

            dumpInstructions(m_output_file.get(), riscv_branch, label_false.top());
        }


    }else if(p_un_op.getOp() == Operator::kNegOp){

        if(!p_un_op.getInferredType()->isReal()){
            constexpr const char*const riscv_op = 
                "    sub  t0, zero, t0\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n";

            dumpInstructions(m_output_file.get(), riscv_op);
        }/* else{
            constexpr const char*const riscv_op = 
                "    fsub.s  ft0, zero, ft0\n"
                "    addi sp, sp, -4\n"
                "    fsw   t0, 0(sp)\n";

            dumpInstructions(m_output_file.get(), riscv_op);
        } */
        

    }

    
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {

    // printf("function invoc node\n");
    cur_funct_invoc = 1;

    p_func_invocation.visitChildNodes(*this);

    cur_funct_invoc = 0;

    bool is_real = p_func_invocation.getInferredType()->isReal();

    int args_num = p_func_invocation.getArguments().size();

    int tmp_args = args_num - 8;

    for(int i = tmp_args+1; i >= 2; i--){

        if(is_real){
            constexpr const char*const riscv_pop_arg =
                "    flw ft%d, 0(sp)\n"
                "    addi sp, sp, 4\n";

            dumpInstructions(m_output_file.get(), riscv_pop_arg, i);
        }else{
            constexpr const char*const riscv_pop_arg =
                "    lw   t%d, 0(sp)\n"
                "    addi sp, sp, 4\n";

            dumpInstructions(m_output_file.get(), riscv_pop_arg, i);
        }
        
    }

    args_num = args_num >= 8 ? 8 : args_num;

    for(int i = args_num-1; i >= 0; i--){

        if(is_real){
            constexpr const char*const riscv_pop_arg =
                "    flw fa%d, 0(sp)\n"
                "    addi sp, sp, 4\n";

            dumpInstructions(m_output_file.get(), riscv_pop_arg, i);
        }else{
            constexpr const char*const riscv_pop_arg =
                "    lw   a%d, 0(sp)\n"
                "    addi sp, sp, 4\n";

            dumpInstructions(m_output_file.get(), riscv_pop_arg, i);
        }
        
    }

    if(p_func_invocation.getInferredType()->isVoid()){
        constexpr const char*const riscv_jmp_funct =
            "    jal ra, %s\n";

        const char* funct_name = p_func_invocation.getNameCString();

        dumpInstructions(m_output_file.get(), riscv_jmp_funct, funct_name);

    }else if(is_real){
        constexpr const char*const riscv_jmp_funct =
            "    jal  ra, %s\n"
            "    addi sp, sp, -4\n"
            "    fsw fa0, 0(sp)\n";

        const char* funct_name = p_func_invocation.getNameCString();

        dumpInstructions(m_output_file.get(), riscv_jmp_funct, funct_name);
    }else{
        constexpr const char*const riscv_jmp_funct =
            "    jal  ra, %s\n"
            "    mv   t0, a0\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        const char* funct_name = p_func_invocation.getNameCString();

        dumpInstructions(m_output_file.get(), riscv_jmp_funct, funct_name);
    }

    bool is_bool = p_func_invocation.getInferredType()->isBool();

    // printf("function invocation: %d\n", is_bool);

    if(is_bool && cur_if){
        constexpr const char*const riscv_op =
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    li   t1, 0\n" 
            "    ble  t0, t1, L%d\n";

        dumpInstructions(m_output_file.get(), riscv_op, label_false.top());
    }
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {

    const char* var_name = p_variable_ref.getNameCString();
    std::string var_name_str = p_variable_ref.getName();
    size_t var_level = m_symbol_manager_ptr->lookup(var_name)->getLevel();
    
    bool is_real = p_variable_ref.getInferredType()->isReal();
    bool is_bool = p_variable_ref.getInferredType()->isBool();
    bool not_array = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->isScalar();
    // printf("var_ref: %s, %d, %d\n", p_variable_ref.getNameCString(), not_array, cur_assign_node);

    if(cur_for && !loop_var_found.top()){
        loop_var.push(var_name_str);
        loop_var_found.pop();
        loop_var_found.push(1);
        // printf("loop_var: %s\n", loop_var.c_str());
    }

    if(!not_array && cur_assign_node){
        int pos = 0;

        std::vector<int> sz_vec(m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions().size());

        // printf("var_name: %s\n", var_name);
            
        int accum = 1;
        for(int i = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions().size()-1; i >= 0; i--){
            sz_vec[i] = accum;
            accum *= m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions()[i];
        }

        // sz_vec[0] = accum;
        // printf("size: %d\n", p_assignment.getLvalue().getIndices().size()); 
        cur_arr = 1;

        for(int i = 0; i < (int)p_variable_ref.getIndices().size(); i++){
            p_variable_ref.visitChildNodes(*this);
        }

        for(int i = 0; i < (int)p_variable_ref.getIndices().size(); i++){
            // printf("dim%d : %d, %d\n", i, arr_dims[i], sz_vec[i]);
            
            pos += arr_dims[i] * sz_vec[i];
        }

        cur_arr = 0;

        arr_dims.clear();
        // printf("array dim: %d\n", arr_dims[0]);
        arr_dims.resize(0);

        // printf("pos: %d\n", pos);

        constexpr const char*const assign_left =
            "    addi t0, s0, -%d\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";
            
        dumpInstructions(m_output_file.get(), assign_left, mem_pos_map[var_name_str] + pos*4);
                
    }

    if(var_level == 0 && is_bool && cur_if){
        constexpr const char*const riscv_var_ref = 
            "    la   t0, %s\n"
            "    lw   t1, 0(t0)\n"
            "    mv   t0, t1\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        dumpInstructions(m_output_file.get(), riscv_var_ref, var_name);

        constexpr const char*const riscv_bin_op = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    li   t1, 0\n"
            "    ble  t1, t0, L%d\n";

        dumpInstructions(m_output_file.get(), riscv_bin_op, label_false.top());
    }else if(var_level > 0 && is_bool && cur_if){

        int addr = 0;
        std::string var_name_str = p_variable_ref.getName();
        addr -= mem_pos_map[var_name_str];

        constexpr const char*const riscv_var_ref = 
            "    addi t0, s0, %d\n"
            "    lw   t1, 0(t0)\n"
            "    mv   t0, t1\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        dumpInstructions(m_output_file.get(), riscv_var_ref, addr);

        constexpr const char*const riscv_bin_op = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    li   t1, 0\n"
            "    ble  t0, t1, L%d\n";

        dumpInstructions(m_output_file.get(), riscv_bin_op, label_false.top());

    }else if(var_level == 0 && !cur_assign_node){

        if(is_real){
            /* float global */
        }else{
            constexpr const char*const riscv_var_ref = 
                "    la   t0, %s\n"
                "    lw   t1, 0(t0)\n"
                "    mv   t0, t1\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n";

            dumpInstructions(m_output_file.get(), riscv_var_ref, var_name);
        }
        
    }else if(var_level > 0 && !cur_assign_node){
        int addr = 0;
        std::string var_name_str = p_variable_ref.getName();
        addr -= mem_pos_map[var_name_str];
        // printf("here\n");

        if(is_real){
            constexpr const char*const riscv_var_ref = 
                "    addi t0, s0, %d\n"
                "    flw ft1, 0(t0)\n"
                "    addi sp, sp, -4\n"
                "    fsw ft1, 0(sp)\n";

            dumpInstructions(m_output_file.get(), riscv_var_ref, addr);
        }else{
            
            bool not_array = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->isScalar();

            int pos = 0;

            if(!not_array){
                
                cur_arr = 1;
                // printf("%d\n", (int)p_variable_ref.getIndices().size());

                p_variable_ref.visitChildNodes(*this);

                cur_arr = 0;

                std::vector<int> sz_vec(m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions().size());
                
                int accum = 1;
                for(int i = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions().size()-1; i >= 0; i--){
                    sz_vec[i] = accum;
                    accum *= m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions()[i];
                }

                // sz_vec[0] = accum;

                for(int i = 0; i < (int)p_variable_ref.getIndices().size(); i++){
                    // printf("dim%d : %d, %d\n", i, arr_dims[i], sz_vec[i]);
                    
                    pos += arr_dims[i] * sz_vec[i];
                }
                
                // printf("pos: %d\n", pos);

                arr_dims.clear();
                
                // printf("array dim: %d\n", arr_dims[0]);
                arr_dims.resize(0);
            }
            

            if(!not_array && (cur_funct_invoc || cur_read) ){
                constexpr const char*const riscv_arr = 
                    "    addi t0, s0, %d\n"
                    "    li   t1, %d\n"
                    "    sub  t0, t0, t1\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t0, 0(sp)\n";

                dumpInstructions(m_output_file.get(), riscv_arr, addr, pos*4);

            }else if(!not_array && cur_funct){
                constexpr const char*const riscv_arr = 
                    "    addi t0, s0, %d\n"
                    "    lw   t1, 0(t0)\n"
                    "    mv   t0, t1\n"
                    "    li   t1, %d\n"
                    "    sub  t0, t0, t1\n"
                    "    lw   t1, 0(t0)\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t1, 0(sp)\n";

                dumpInstructions(m_output_file.get(), riscv_arr, addr, pos*4);
            }else if(!not_array){

                constexpr const char*const riscv_arr = 
                    "    addi t0, s0, %d\n"
                    "    li   t1, %d\n"
                    "    sub  t0, t0, t1\n"
                    "    lw   t1, 0(t0)\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t1, 0(sp)\n";

                dumpInstructions(m_output_file.get(), riscv_arr, addr, pos*4);
            }else{
                constexpr const char*const riscv_var_ref = 
                    "    addi t0, s0, %d\n"
                    "    lw   t1, 0(t0)\n"
                    "    mv   t0, t1\n"
                    "    addi sp, sp, -4\n"
                    "    sw   t0, 0(sp)\n";

                dumpInstructions(m_output_file.get(), riscv_var_ref, addr);
            }
        }
        
    }

    cur_assign_node = 0;
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    
    const char* var_name = p_assignment.getLvalue().getNameCString();
    std::string var_name_str = p_assignment.getLvalue().getName();
    size_t var_level = m_symbol_manager_ptr->lookup(var_name)->getLevel();
    bool is_real = p_assignment.getLvalue().getInferredType()->isReal();
    bool not_array = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->isScalar();

    if(var_level == 0){
        /* prepare address of global variable */
        constexpr const char*const assign_left = 
            "    la   t0, %s\n" 
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";

        dumpInstructions(m_output_file.get(), assign_left, var_name);
    }else if(not_array){
        
        constexpr const char*const assign_left =
            "    addi t0, s0, -%d\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n";
            
        dumpInstructions(m_output_file.get(), assign_left, mem_pos_map[var_name_str]);
        
    }

    cur_assign_node = 1;

    if(p_assignment.getLvalue().getInferredType()->isBool()){
        bool_assign = 1;
    }

    p_assignment.visitChildNodes(*this);

    if(cur_for){
        for_end_cond = 1;
    }

    if(!is_real){
        constexpr const char*const load_value = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n";

        dumpInstructions(m_output_file.get(), load_value);

        constexpr const char*const load_addr =
            "    lw   t1, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    sw   t0, 0(t1)\n";

        dumpInstructions(m_output_file.get(), load_addr);
    }else{

        constexpr const char*const load_value =
            "    flw  ft1, 0(sp)\n"
            "    addi sp, sp, 4\n";

        dumpInstructions(m_output_file.get(), load_value);

        constexpr const char*const load_addr = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    fsw ft1, 0(t0)\n";

        dumpInstructions(m_output_file.get(), load_addr);
    }
    
}

void CodeGenerator::visit(ReadNode &p_read) {
    // p_read.visitChildNodes(*this);

    const char* var_name = p_read.getTarget().getNameCString();
    std::string var_name_str = p_read.getTarget().getName();
    size_t var_level = m_symbol_manager_ptr->lookup(var_name)->getLevel();
    bool not_array = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->isScalar();

    if(var_level == 0){
        constexpr const char*const riscv_read =
            "    la   t0, %s\n"
            "    addi sp, sp, -4\n"
            "    sw   t0, 0(sp)\n"
            "    jal  ra, readInt\n"
            "    lw   t0, 0(sp)\n" 
            "    addi sp, sp, 4\n"
            "    sw   a0, 0(t0)\n";

        dumpInstructions(m_output_file.get(), riscv_read, var_name);
    }else if(var_level > 0){
        
        if(not_array){
            constexpr const char*const riscv_read =
                "    addi t0, s0, -%d\n"
                "    addi sp, sp, -4\n"
                "    sw   t0, 0(sp)\n"
                "    jal  ra, readInt\n"
                "    lw   t0, 0(sp)\n" 
                "    addi sp, sp, 4\n"
                "    sw   a0, 0(t0)\n";

            dumpInstructions(m_output_file.get(), riscv_read, mem_pos_map[var_name_str]);
        }else if(!not_array){
                
            cur_arr = 1;
            cur_read = 1;
            // printf("%d\n", (int)p_variable_ref.getIndices().size());

            p_read.visitChildNodes(*this);

            cur_arr = 0;
            cur_read = 0;

            int pos = 0;

            std::vector<int> sz_vec(m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions().size());
            
            int accum = 1;
            for(int i = m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions().size()-1; i >= 0; i--){
                sz_vec[i] = accum;
                accum *= m_symbol_manager_ptr->lookup(var_name_str)->getTypePtr()->getDimensions()[i];
            }

            // sz_vec[0] = accum;

            for(int i = 0; i < (int)p_read.getTarget().getIndices().size(); i++){
                // printf("dim%d : %d, %d\n", i, arr_dims[i], sz_vec[i]);
                
                pos += arr_dims[i] * sz_vec[i];
            }
            
            // printf("pos: %d\n", pos);

            arr_dims.clear();
            
            // printf("array dim: %d\n", arr_dims[0]);
            arr_dims.resize(0);

            constexpr const char*const riscv_read =
                "    jal  ra, readInt\n"
                "    lw   t0, 0(sp)\n" 
                "    addi sp, sp, 4\n"
                "    sw   a0, 0(t0)\n";

            dumpInstructions(m_output_file.get(), riscv_read, mem_pos_map[var_name_str]+pos*4);
        }
        
    }

    
}

void CodeGenerator::visit(IfNode &p_if) {

    // printf("if node\n");

    cur_if = 1;

    label_true.push(label);
    label_false.push(++label);
    label++;

    p_if.visitChildNodes(*this);

    cur_if = 0;
    cond_body = 0;

    constexpr const char*const riscv_label =
        "L%d:\n";

    dumpInstructions(m_output_file.get(), riscv_label, label_true.top());
    label_true.pop();

}

void CodeGenerator::visit(WhileNode &p_while) {

    cur_while = 1;
    if(label == 1){
        constexpr const char*const riscv_finish_label =
            "L%d:\n";

        dumpInstructions(m_output_file.get(), riscv_finish_label, label);
        label++;
    }

    label_true.push(label);
    // printf("cur_label: %d\n", label);
    label++;
    label_false.push(label);
    // printf("cur_label: %d\n", label);
    label++;

    p_while.visitChildNodes(*this);

    cur_while = 0;

    constexpr const char*const riscv_finish_label =
        "    j    L%d\n"
        "L%d:\n";

    dumpInstructions(m_output_file.get(), riscv_finish_label, label_false.top()-2, label_false.top());
    label_false.pop();
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_for.getSymbolTable());

    cur_for = 1;

    loop_var_found.push(0);

    if(label == 1){
        constexpr const char*const riscv_finish_label =
            "L%d:\n";

        dumpInstructions(m_output_file.get(), riscv_finish_label, label);
        label++;
    }

    p_for.visitChildNodes(*this);

    cur_for = 0;

    constexpr const char*const update_loop_var =
        "    addi t0, s0, -%d\n"
        "    addi sp, sp, -4\n"
        "    sw   t0, 0(sp)\n"
        "    lw   t0, -%d(s0)\n"
        "    addi sp, sp, -4\n"
        "    sw   t0, 0(sp)\n"
        "    li   t0, 1\n"
        "    addi sp, sp, -4\n"
        "    sw   t0, 0(sp)\n"
        "    lw   t0, 0(sp)\n"          
        "    addi sp, sp, 4\n"
        "    lw   t1, 0(sp)\n" 
        "    addi sp, sp, 4\n"
        "    add  t0, t1, t0\n"       
        "    addi sp, sp, -4\n"
        "    sw   t0, 0(sp)\n"   
        "    lw   t0, 0(sp)\n"        
        "    addi sp, sp, 4\n"
        "    lw   t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    sw   t0, 0(t1)\n";
    
    dumpInstructions(m_output_file.get(), update_loop_var, mem_pos_map[loop_var.top()], mem_pos_map[loop_var.top()]);
    loop_var.pop();
    loop_var_found.pop();
    loop_var_found.push(0);

    constexpr const char*const riscv_label =
        "    j    L%d\n"
        "L%d:\n";

    dumpInstructions(m_output_file.get(), riscv_label, label_false.top()-2, label_false.top());
    label_false.pop();

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {

    // printf("return\n");
    
    p_return.visitChildNodes(*this);

    bool is_real = p_return.getReturnValue().getInferredType()->isReal();

    if(is_real){
        constexpr const char*const load_ret =
            "    flw fa0, 0(sp)\n"
            "    addi sp, sp, 4\n";

            dumpInstructions(m_output_file.get(), load_ret);

    }else{
        constexpr const char*const load_value = 
            "    lw   t0, 0(sp)\n"
            "    addi sp, sp, 4\n";

        dumpInstructions(m_output_file.get(), load_value);

        constexpr const char*const mov_to_ret =
            "    mv   a0, t0\n";

        dumpInstructions(m_output_file.get(), mov_to_ret);
    }
    
}
