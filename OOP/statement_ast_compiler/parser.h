#pragma once
#include "ast.h"
#include <vector>
#include <string>

struct Token {
    int type;
    std::string value;
};

class Parser {
public:
    Parser(const vector<Token>& tokens);

    shared_ptr<Node> parse();

private:
    vector<Token> tokens;
    int pos;

    shared_ptr<Node> parseExpression();
    shared_ptr<Node> parseTerm();
    shared_ptr<Node> parseFactor();

    shared_ptr<Node> parseStatement();
    shared_ptr<Node> parseIf();
    shared_ptr<Node> parseWhile();
};
