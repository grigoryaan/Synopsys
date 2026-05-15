#pragma once
#include <string>
#include <vector>

enum TokenType {
    NUMBER, VARIABLE,
    PLUS, MINUS, MUL, DIV,
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    SEMICOLON, ASSIGN,
    LT, GT, LE, GE, EQ, NEQ,
    IF, ELSE, WHILE, DO,
    SWITCH, CASE, DEFAULT, COLON,
    RETURN_KW,
    BREAK_KW,
    END
};

struct Token {
    TokenType   type;
    std::string value;
};

extern std::vector<Token> tokens;
extern int pos;

void tokenize(const std::string& input);
