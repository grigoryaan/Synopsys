#include "parser.h"
#include "token.h"
#include <stdexcept>

// ── helpers 
static Token& cur()  { return tokens[pos]; }
static Token  eat()  { return tokens[pos++]; }

static Token expect(TokenType t, const std::string& ctx) {
    if (cur().type != t)
        throw std::runtime_error("Parse error at '" + cur().value + "' in " + ctx);
    return eat();
}

// ── Expression parsing 
NodePtr parseFactor() {
    Token t = eat();

    if (t.type == NUMBER) {
        auto n = std::make_shared<Node>(NodeKind::NUMBER, t.value);
        return n;
    }
    if (t.type == VARIABLE) {
        auto n = std::make_shared<Node>(NodeKind::VARIABLE, t.value);
        return n;
    }
    if (t.type == LPAREN) {
        auto n = parseExpression();
        expect(RPAREN, "factor");
        return n;
    }
    throw std::runtime_error("Unexpected token in factor: '" + t.value + "'");
}

// term   
NodePtr parseTerm() {
    auto node = parseFactor();
    while (cur().type == MUL || cur().type == DIV) {
        std::string op = eat().value;
        auto right = parseFactor();
        auto n = std::make_shared<Node>(NodeKind::BINOP, op);
        n->left  = node;
        n->right = right;
        node = n;
    }
    return node;
}

// additive 
static NodePtr parseAdditive() {
    auto node = parseTerm();
    while (cur().type == PLUS || cur().type == MINUS) {
        std::string op = eat().value;
        auto right = parseTerm();
        auto n = std::make_shared<Node>(NodeKind::BINOP, op);
        n->left  = node;
        n->right = right;
        node = n;
    }
    return node;
}

// comparison := additive 
NodePtr parseComparison() {
    auto node = parseAdditive();
    TokenType tt = cur().type;
    if (tt == LT || tt == GT || tt == LE || tt == GE || tt == EQ || tt == NEQ) {
        std::string op = eat().value;
        auto right = parseAdditive();
        auto n = std::make_shared<Node>(NodeKind::BINOP, op);
        n->left  = node;
        n->right = right;
        return n;
    }
    return node;
}

// expression := comparison
NodePtr parseExpression() {
    return parseComparison();
}

// ── Statement parsing
NodePtr parseBlock() {
    expect(LBRACE, "block");
    auto block = std::make_shared<Node>(NodeKind::BLOCK);
    while (cur().type != RBRACE && cur().type != END) {
        block->stmts.push_back(parseStatement());
    }
    expect(RBRACE, "block end");
    return block;
}

NodePtr parseIfStmt() {
    eat(); 
    expect(LPAREN, "if condition");
    auto cond = parseExpression();
    expect(RPAREN, "if condition");
    auto body = parseBlock();

    auto node = std::make_shared<Node>(NodeKind::IF, "if");
    node->cond = cond;
    node->body = body;

    if (cur().type == ELSE) {
        eat(); 
        node->alt = parseBlock();
    }
    return node;
}

NodePtr parseWhileStmt() {
    eat(); 
    expect(LPAREN, "while condition");
    auto cond = parseExpression();
    expect(RPAREN, "while condition");
    auto body = parseBlock();

    auto node = std::make_shared<Node>(NodeKind::WHILE, "while");
    node->cond = cond;
    node->body = body;
    return node;
}

NodePtr parseReturn() {
    eat(); // consume 'return'
    auto expr = parseExpression();
    expect(SEMICOLON, "return");
    auto node = std::make_shared<Node>(NodeKind::RETURN, "return");
    node->left = expr;
    return node;
}

NodePtr parseAssignOrExpr() {
    if (cur().type == VARIABLE && pos + 1 < (int)tokens.size()
        && tokens[pos + 1].type == ASSIGN) {
        std::string name = eat().value; 
        eat();                         
        auto val = parseExpression();
        expect(SEMICOLON, "assignment");
        auto node = std::make_shared<Node>(NodeKind::ASSIGN, name);
        node->right = val;
        return node;
    }
    // Plain expression statement
    auto expr = parseExpression();
    expect(SEMICOLON, "expression statement");
    auto node = std::make_shared<Node>(NodeKind::EXPR_STMT);
    node->left = expr;
    return node;
}

NodePtr parseStatement() {
    switch (cur().type) {
        case IF:        return parseIfStmt();
        case WHILE:     return parseWhileStmt();
        case RETURN_KW: return parseReturn();
        default:        return parseAssignOrExpr();
    }
}

NodePtr parseProgram() {
    auto prog = std::make_shared<Node>(NodeKind::BLOCK, "program");
    while (cur().type != END) {
        prog->stmts.push_back(parseStatement());
    }
    return prog;
}
