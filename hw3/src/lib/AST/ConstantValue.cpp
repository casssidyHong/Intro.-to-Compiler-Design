#include "AST/ConstantValue.hpp"
#include <string>

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, Variables *cons)
    : ExpressionNode{line, col}, cons(cons) {}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

const char* ConstantValueNode::getNameCString() {
    switch (cons->type) {
        case DataType::int_type:
            return std::to_string(cons->int_value).c_str();
        case DataType::real_type:
            return std::to_string(cons->real_value).c_str();
        case DataType::string_type:
            return cons->string_value;
        case DataType::bool_type:
            switch (cons->boolean_type) {
                case BooleanType::true_bool:
                    return "true";
                case BooleanType::false_bool:
                    return "false";
            }
            break;
    }
    return "";
}

