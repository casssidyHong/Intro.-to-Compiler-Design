#include "AST/decl.hpp"

// test
DeclNode::DeclNode(const uint32_t line, const uint32_t col, const char* type) : AstNode{line, col}{

}

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, const char* type, VariableNode* id_list, bool isParam)
    : AstNode{line, col} {

    VariableNode* id_ptr = id_list;

    while(id_ptr != nullptr){
        // this->vars.push_back(id_ptr);
        this->vars.insert(this->vars.begin(), id_ptr);
        // this->vars.resize(1, nullptr);
        // printf("size: %d\n", this->vars.size());
        AstNode* next_var = id_ptr->getNextNode();
        id_ptr = dynamic_cast<VariableNode*>(next_var);
    }

    for(VariableNode* id_node : this->vars){
        id_node->setVarType(type);
        id_node->setParam(isParam);
    }
    
}

DeclNode::DeclNode(const uint32_t line, const uint32_t col, ConstantValueNode* value_node, VariableNode* id_list)
    : AstNode{line, col} {
    
    VariableNode* id_ptr = id_list;

    while(id_ptr != nullptr){
        this->vars.insert(this->vars.begin(), id_ptr);
        AstNode* next_var = id_ptr->getNextNode();
        id_ptr = dynamic_cast<VariableNode*>(next_var);
    }

    for(auto id_node : this->vars){
        id_node->setVarType(value_node->getType());
        id_node->assignValue(value_node);
    }
}

// TODO
//DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {
    
    std::printf("declaration <line: %u, col: %u>\n", location.line,
                location.col);
}

AstNode* DeclNode::getNextNode(){
    return this->next_decl;
}

void DeclNode::setNextNode(AstNode* next_node){
    this->next_decl = dynamic_cast<DeclNode*>(next_node);
}

void DeclNode::accept(AstNodeVisitor& p_visitor){
    // AstDumper* ast_dump = dynamic_cast<AstDumper*>(&p_visitor);
    p_visitor.visit(*this);
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    for(auto var : this->vars){
        var->accept(p_visitor);
    }
}

std::vector<VariableNode*>& DeclNode::getVarsVec(){
    return this->vars;
}