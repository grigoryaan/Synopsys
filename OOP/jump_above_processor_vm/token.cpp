#include "token.h"
#include <cctype>
#include <stdexcept>

std::vector<Token> tokens;
int pos = 0;

void tokenize(const std::string& input) {
    tokens.clear();

    for (int i = 0; i < (int)input.size(); ) {
        if (std::isspace(input[i])) { ++i; continue; }

        if (std::isdigit(input[i])) {
            std::string num;
            while (i < (int)input.size() && (std::isdigit(input[i]) || input[i] == '.'))
                num += input[i++];
            tokens.push_back({NUMBER, num});
            continue;
        }

        if (std::isalpha(input[i]) || input[i] == '_') {
            std::string word;
            while (i < (int)input.size() && (std::isalnum(input[i]) || input[i] == '_'))
                word += input[i++];

            if      (word == "if")      tokens.push_back({IF,       word});
            else if (word == "else")    tokens.push_back({ELSE,     word});
            else if (word == "while")   tokens.push_back({WHILE,    word});
            else if (word == "do")      tokens.push_back({DO,       word});
            else if (word == "switch")  tokens.push_back({SWITCH,   word});
            else if (word == "case")    tokens.push_back({CASE,     word});
            else if (word == "default") tokens.push_back({DEFAULT,  word});
            else if (word == "return")  tokens.push_back({RETURN_KW,word});
            else if (word == "break")   tokens.push_back({BREAK_KW, word});
            else                        tokens.push_back({VARIABLE, word});
            continue;
        }

        if (i + 1 < (int)input.size()) {
            std::string two = {input[i], input[i+1]};
            if (two == "<=") { tokens.push_back({LE,  two}); i+=2; continue; }
            if (two == ">=") { tokens.push_back({GE,  two}); i+=2; continue; }
            if (two == "==") { tokens.push_back({EQ,  two}); i+=2; continue; }
            if (two == "!=") { tokens.push_back({NEQ, two}); i+=2; continue; }
        }

        switch (input[i]) {
            case '+': tokens.push_back({PLUS,      "+"}); break;
            case '-': tokens.push_back({MINUS,     "-"}); break;
            case '*': tokens.push_back({MUL,       "*"}); break;
            case '/': tokens.push_back({DIV,       "/"}); break;
            case '(': tokens.push_back({LPAREN,    "("}); break;
            case ')': tokens.push_back({RPAREN,    ")"}); break;
            case '{': tokens.push_back({LBRACE,    "{"}); break;
            case '}': tokens.push_back({RBRACE,    "}"}); break;
            case ';': tokens.push_back({SEMICOLON, ";"}); break;
            case ':': tokens.push_back({COLON,     ":"}); break;
            case '<': tokens.push_back({LT,        "<"}); break;
            case '>': tokens.push_back({GT,        ">"}); break;
            case '=': tokens.push_back({ASSIGN,    "="}); break;
            default:
                throw std::runtime_error(std::string("Unknown char: ") + input[i]);
        }
        ++i;
    }
    tokens.push_back({END, ""});
}
