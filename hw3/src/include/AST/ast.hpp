#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H

#include <cstdint>
#include <vector>
#include <string>

#include <visitor/AstNodeVisitor.hpp>
#include <AST/AstDumper.hpp>

class AstNodeVisitor;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

class AstNode {
  protected:
    Location location;

  public:
    virtual ~AstNode() = 0;
    AstNode(const uint32_t line, const uint32_t col);

    const Location &getLocation() const;

    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor){};

    virtual void print() = 0;
};

enum class BooleanType{
  true_bool,
  false_bool
};

// union Value{
//   int int_value;
//   double real_value;
//   char *string_value;
// };

enum class DataType{
  int_type,
  real_type,
  string_type,
  bool_type,
  void_
};

enum class OperatorType{
  equal_op,
  not_equal_op,
  smaller_op,
  smaller_or_equal,
  bigger_op,
  bigger_or_equal,
  assign_op,
  and_op,
  or_op,
  not_op,
  plus_op,
  minus_op,
  multiply_op,
  devide_op,
  mod_op,
};

struct Id {
  uint32_t line_number;
  uint32_t col_number;
  char* name;
};

struct Operator {
  OperatorType operator_type;
};

struct Variables {
  uint32_t line_number;
  uint32_t col_number;
  BooleanType boolean_type;
  DataType type;
  int int_value;
  double real_value;
  char* string_value;
  std::vector<int> arr_num;
};

struct NodeType {
  AstNode* ast_node;
  Variables* type;
  int cnt;
};

#endif
