#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                     Operator *op, AstNode *expr)
    : ExpressionNode{line, col}, op(op), expr(expr) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr != NULL)
        expr->accept(p_visitor);
}

const char* UnaryOperatorNode::getNameCType() {
    std::string op_type;

    switch (op->operator_type) {
        case OperatorType::minus_op:
            op_type = "neg";
            break;
        case OperatorType::not_op:
            op_type = "not";
            break;
    }
    return op_type.c_str();
}

