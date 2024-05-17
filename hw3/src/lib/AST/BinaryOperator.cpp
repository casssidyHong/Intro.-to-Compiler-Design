#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                        Operator *op, 
                                        AstNode *left, 
                                        AstNode *right)
    : ExpressionNode{line, col}, op(op), left(left), right(right) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::accept(AstNodeVisitor &visitor){
    visitor.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(left != NULL)
        left->accept(p_visitor);
    if(right != NULL)
        right->accept(p_visitor);
}

const char* BinaryOperatorNode::getNameCType() {
    std::string oper_type;

    switch (op->operator_type) {
        case OperatorType::equal_op:
            oper_type = "=";
            break;
        case OperatorType::not_equal_op:
            oper_type = "<>";
            break;
        case OperatorType::smaller_op:
            oper_type = "<";
            break;
        case OperatorType::smaller_or_equal:
            oper_type = "<=";
            break;
        case OperatorType::bigger_op:
            oper_type = ">";
            break;
        case OperatorType::bigger_or_equal:
            oper_type = ">=";
            break;
        case OperatorType::and_op:
            oper_type = "and";
            break;
        case OperatorType::or_op:
            oper_type = "or";
            break;
        case OperatorType::plus_op:
            oper_type = "+";
            break;
        case OperatorType::minus_op:
            oper_type = "-";
            break;
        case OperatorType::multiply_op:
            oper_type = "*";
            break;
        case OperatorType::devide_op:
            oper_type = "/";
            break;
        case OperatorType::mod_op:
            oper_type = "mod";
            break;
    }
    return oper_type.c_str();
}
