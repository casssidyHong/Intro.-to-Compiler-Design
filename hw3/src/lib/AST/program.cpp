#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *name,const char *return_type,
                         std::vector <AstNode*> *decl,
                         std::vector <AstNode*> *func,
                         AstNode *body)
    : AstNode{line, col}, name(name), return_type(return_type),
      decl(decl), func(func), body(body) {}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print()
{
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");
    
    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();

}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
    // TODO
    if(decl != NULL){
        for (auto &decl : *decl)
            decl->accept(p_visitor);
    }
    if(func != NULL){
        for (auto &func : *func)
            func->accept(p_visitor);
    }
    body->accept(p_visitor);
}
