#pragma once
#include <memory>
#include <string>
#include <vector>

using namespace std;

enum class NodeType {
    NUMBER,
    VARIABLE,
    BINARY_OP,
    IF,
    WHILE
};

struct Node {
    NodeType type;
    string value;

    shared_ptr<Node> left;
    shared_ptr<Node> right;

    shared_ptr<Node> condition;
    shared_ptr<Node> body;
    shared_ptr<Node> elseBranch;

    Node(NodeType t, string v = "")
        : type(t), value(v) {}
};
