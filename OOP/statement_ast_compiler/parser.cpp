#include "parser.h"

Parser::Parser(const vector<Token>& t)
    : tokens(t), pos(0) {}

shared_ptr<Node> Parser::parse() {
    return parseStatement();
}

shared_ptr<Node> Parser::parseIf() {


    pos++;
  
    auto cond = parseExpression();

    auto body = parseStatement();

    shared_ptr<Node> elseNode = nullptr;

    if (tokens[pos].value == "else") {
        pos++;
        elseNode = parseStatement();
    }

    auto node = make_shared<Node>(NodeType::IF);
    node->condition = cond;
    node->body = body;
    node->elseBranch = elseNode;

    return node;
}


shared_ptr<Node> Parser::parseWhile() {

    pos++; 

    auto cond = parseExpression();
    auto body = parseStatement();

    auto node = make_shared<Node>(NodeType::WHILE);
    node->condition = cond;
    node->body = body;

    return node;
}

shared_ptr<Node> Parser::parseStatement() {

    if (tokens[pos].value == "if")
        return parseIf();

    if (tokens[pos].value == "while")
        return parseWhile();

    return parseExpression();
}
