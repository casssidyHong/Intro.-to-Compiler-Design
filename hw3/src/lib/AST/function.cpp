#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                           const char *name, 
                           std::vector <AstNode*> *decl, 
                           std::vector <Variables*> *para,
                           Variables *return_type, 
                           AstNode *comp)
    : AstNode{line, col}, name(name), decl(decl), para(para),
      return_type(return_type), comp(comp) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}


void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(decl != NULL){
        for (auto &decl : *decl) {
            decl->accept(p_visitor);
        }
    }

    if(comp != NULL){
        comp->accept(p_visitor);
    }
}

const char *FunctionNode::getNameCString(){
    return name;
}

std::string FunctionNode::getNameCParam() {
    std::string func_parameter = "(";

    if (!para->empty()) {
        for (size_t i = 0; i < para->size(); ++i) {
            switch (para->at(i)->type) {
                case DataType::int_type:
                    func_parameter += "integer";
                    break;
                case DataType::real_type:
                    func_parameter += "real";
                    break;
                case DataType::string_type:
                    func_parameter += "string";
                    break;
                case DataType::bool_type:
                    func_parameter += "boolean";
                    break;
            }

            if (!para->at(i)->arr_num.empty()) {
                func_parameter += " ";
                for (size_t j = 0; j < para->at(i)->arr_num.size(); ++j) {
                    func_parameter += "[" + std::to_string(para->at(i)->arr_num[j]) + "]";
                }
            }
            if (i != para->size() - 1)
                func_parameter += ", ";
        }
    }
    func_parameter += ")";
    return func_parameter;
}

const char* FunctionNode::getNameCType() {
    std::string func_parameter = "";

    switch (return_type->type) {
        case DataType::int_type:
            func_parameter += "integer";
            break;
        case DataType::real_type:
            func_parameter += "real";
            break;
        case DataType::string_type:
            func_parameter += "string";
            break;
        case DataType::bool_type:
            func_parameter += "boolean";
            break;
        case DataType::void_:
            func_parameter += "void";
            break;
    }
    return func_parameter.c_str();
}
