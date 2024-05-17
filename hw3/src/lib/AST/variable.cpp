#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                           const char *name, 
                           Variables *type,
                           AstNode *cons)
    : AstNode{line, col}, name(name), type(type), cons(cons) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(cons != NULL)
        cons->accept(p_visitor);
}

const char *VariableNode::getNameCString(){
    return name;
}

const char *VariableNode::getNameCType(){
    std::string var_type;
    switch (type->type) {
        case DataType::int_type:
            var_type = "integer";
            break;
        case DataType::real_type:
            var_type = "real";
            break;
        case DataType::string_type:
            var_type = "string";
            break;
        case DataType::bool_type:
            var_type = "boolean";
            break;
    }

    if(type->arr_num.size() > 0){
        var_type += " ";
        for(int i = 0; i < type->arr_num.size(); i++){
            var_type += "[";
            var_type += std::to_string(type->arr_num[i]);
            var_type += "]";
        }        
    }

    return var_type.c_str();
}   