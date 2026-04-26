#include "ast_print.h"
#include <iostream>

static std::string kindName(NodeKind k) {
    switch (k) {
        case NodeKind::NUMBER:    return "NUMBER";
        case NodeKind::VARIABLE:  return "VARIABLE";
        case NodeKind::BINOP:     return "BINOP";
        case NodeKind::ASSIGN:    return "ASSIGN";
        case NodeKind::IF:        return "IF";
        case NodeKind::WHILE:     return "WHILE";
        case NodeKind::BLOCK:     return "BLOCK";
        case NodeKind::RETURN:    return "RETURN";
        case NodeKind::EXPR_STMT: return "EXPR_STMT";
        default: return "UNKNOWN";
    }
}

void printAST(NodePtr node, int indent) {
    if (!node) return;
    std::string pad(indent * 2, ' ');

    std::cout << pad << "[" << kindName(node->kind) << "]";
    if (!node->value.empty()) std::cout << " \"" << node->value << "\"";
    std::cout << "\n";

    // Expression children
    if (node->left)  { std::cout << pad << "  left:\n";  printAST(node->left,  indent + 2); }
    if (node->right) { std::cout << pad << "  right:\n"; printAST(node->right, indent + 2); }

    // Statement children
    if (node->cond) { std::cout << pad << "  cond:\n";  printAST(node->cond, indent + 2); }
    if (node->body) { std::cout << pad << "  body:\n";  printAST(node->body, indent + 2); }
    if (node->alt)  { std::cout << pad << "  else:\n";  printAST(node->alt,  indent + 2); }

    // Block statements
    if (!node->stmts.empty()) {
        std::cout << pad << "  stmts:\n";
        for (auto& s : node->stmts)
            printAST(s, indent + 2);
    }
}
