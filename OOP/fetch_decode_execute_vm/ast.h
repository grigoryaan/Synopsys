#pragma once
#include <string>
#include <memory>
#include <vector>

enum class NodeKind {
    NUMBER, VARIABLE, BINOP,
    ASSIGN,
    IF, WHILE, DO_WHILE,
    SWITCH, CASE, DEFAULT_CASE,
    BLOCK, RETURN, BREAK,
    EXPR_STMT
};

struct Node {
    NodeKind kind;
    std::string value;

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::shared_ptr<Node> cond;
    std::shared_ptr<Node> body;
    std::shared_ptr<Node> alt;

    std::vector<std::shared_ptr<Node>> stmts;

    explicit Node(NodeKind k, std::string v = "")
        : kind(k), value(std::move(v)) {}
};

using NodePtr = std::shared_ptr<Node>;
