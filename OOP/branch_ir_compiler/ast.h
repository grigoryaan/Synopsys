#pragma once
#include <string>
#include <memory>
#include <vector>

enum class NodeKind {
    // Expression nodes
    NUMBER,       
    VARIABLE,      
    BINOP,         
    // Statement nodes
    ASSIGN,       
    IF,            
    WHILE,        
    BLOCK,        
    RETURN,        
    EXPR_STMT      
};

struct Node {
    NodeKind kind;
    std::string value;                      

    // Expression children
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    // Statement children
    std::shared_ptr<Node> cond;             
    std::shared_ptr<Node> body;              
    std::shared_ptr<Node> alt;              
    std::vector<std::shared_ptr<Node>> stmts; 

    // Convenience constructors
    explicit Node(NodeKind k, std::string v = "")
        : kind(k), value(std::move(v)) {}
};

using NodePtr = std::shared_ptr<Node>;
