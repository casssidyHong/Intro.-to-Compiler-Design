#ifndef ERROR_MSG_H
#define ERROR_MSG_H

#include <cstring>
#include <vector>
#include "visitor/AstNodeInclude.hpp"

#define REDECL_ERROR 0
#define ARR_DIM_ZERO 1
#define SYM_NOT_FOUND 2
#define SYM_NOT_VAR 3
#define DIM_NOT_INT 4
#define OVER_ARR_SUBSCRIPT 5
#define INVALID_BIN_OP 6
#define INVALID_UN_OP 7
#define FUNCT_NOT_FOUND 8
#define NON_FUNCT_CALL 9
#define INVALID_ARG_NUM 10
#define ARG_TYPE_UNMATCH 11
#define PRINT_NONSCALAR 13
#define READ_NONSCALAR 14
#define READ_VAR_REF_ERR 15
#define ARR_ASSIGN 16
#define CONST_ASSIGN 17
#define LOOP_VAR_ASSIGN 18
#define ASSIGN_TYPE_ERROR 19
#define CONDITION_NOT_BOOL 20
#define LOOP_VAR_DEC 21
#define RET_TYPE_ERROR 22
#define RET_POS_ERROR 23

class ErrorMsg {
    private:
        AstNode* err_node;
        int err;
        const char* err_line;
        const char* sym_name;

    public:
        ErrorMsg(AstNode* err_node, int err, char* err_line, const char* sym_name);

        void setErr(int err);
        int getErr();

        void setSymName(const char* sym_name);
        const char* getSymName();

        void setErrorNode(AstNode* err_node);
        AstNode* getErrorNode();

        void showErrMsg();
};

class ErrorManager {
    private:
        std::vector<ErrorMsg> err_list;
        std::vector<char*> code;

    public:
        ErrorManager() = default;
        void addErrorMsg(AstNode* err_node, int err, int row, const char* sym_name);
        void showErrList();

        void addCodeLine(char* line);
        char* getCodeLine(int row);

        void setCode(std::vector<char*> code);
        std::vector<char*>& getCode();

        bool isDupErr(const char* name);
};


#endif