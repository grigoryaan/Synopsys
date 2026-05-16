#include "parser.h"
#include "token.h"
#include <stdexcept>

static Token& cur()  { return tokens[pos]; }
static Token  eat()  { return tokens[pos++]; }

static Token expect(TokenType t, const std::string& ctx) {
    if (cur().type != t)
        throw std::runtime_error("Parse error at '" + cur().value + "' in " + ctx);
    return eat();
}

NodePtr parseFuncCall(const std::string& name) {
    eat(); // '('
    auto node = std::make_shared<Node>(NodeKind::FUNC_CALL, name);
    while (cur().type != RPAREN && cur().type != END) {
        node->stmts.push_back(parseExpression());
        if (cur().type == COMMA) eat();
    }
    expect(RPAREN, "func call");
    return node;
}

NodePtr parseFactor() {
    if (cur().type == VARIABLE && pos+1 < (int)tokens.size()
        && tokens[pos+1].type == LPAREN) {
        std::string name = eat().value;
        return parseFuncCall(name);
    }
    Token t = eat();
    if (t.type == NUMBER)   return std::make_shared<Node>(NodeKind::NUMBER,   t.value);
    if (t.type == VARIABLE) return std::make_shared<Node>(NodeKind::VARIABLE, t.value);
    if (t.type == LPAREN) {
        auto n = parseExpression();
        expect(RPAREN, "factor");
        return n;
    }
    throw std::runtime_error("Unexpected token in factor: '" + t.value + "'");
}

NodePtr parseTerm() {
    auto node = parseFactor();
    while (cur().type == MUL || cur().type == DIV) {
        std::string op = eat().value;
        auto r = parseFactor();
        auto n = std::make_shared<Node>(NodeKind::BINOP, op);
        n->left = node; n->right = r; node = n;
    }
    return node;
}

static NodePtr parseAdditive() {
    auto node = parseTerm();
    while (cur().type == PLUS || cur().type == MINUS) {
        std::string op = eat().value;
        auto r = parseTerm();
        auto n = std::make_shared<Node>(NodeKind::BINOP, op);
        n->left = node; n->right = r; node = n;
    }
    return node;
}

NodePtr parseComparison() {
    auto node = parseAdditive();
    TokenType tt = cur().type;
    if (tt==LT||tt==GT||tt==LE||tt==GE||tt==EQ||tt==NEQ) {
        std::string op = eat().value;
        auto r = parseAdditive();
        auto n = std::make_shared<Node>(NodeKind::BINOP, op);
        n->left = node; n->right = r; return n;
    }
    return node;
}

NodePtr parseExpression() { return parseComparison(); }

NodePtr parseBlock() {
    expect(LBRACE, "block");
    auto block = std::make_shared<Node>(NodeKind::BLOCK);
    while (cur().type != RBRACE && cur().type != END)
        block->stmts.push_back(parseStatement());
    expect(RBRACE, "block end");
    return block;
}

NodePtr parseIfStmt() {
    eat();
    expect(LPAREN, "if");
    auto cond = parseExpression();
    expect(RPAREN, "if");
    auto body = parseBlock();
    auto node = std::make_shared<Node>(NodeKind::IF, "if");
    node->cond = cond; node->body = body;
    if (cur().type == ELSE) { eat(); node->alt = parseBlock(); }
    return node;
}

NodePtr parseWhileStmt() {
    eat();
    expect(LPAREN, "while");
    auto cond = parseExpression();
    expect(RPAREN, "while");
    auto node = std::make_shared<Node>(NodeKind::WHILE, "while");
    node->cond = cond; node->body = parseBlock();
    return node;
}

NodePtr parseDoWhileStmt() {
    eat();
    auto body = parseBlock();
    expect(WHILE, "do-while");
    expect(LPAREN, "do-while");
    auto cond = parseExpression();
    expect(RPAREN, "do-while");
    expect(SEMICOLON, "do-while");
    auto node = std::make_shared<Node>(NodeKind::DO_WHILE, "do-while");
    node->cond = cond; node->body = body;
    return node;
}

NodePtr parseSwitchStmt() {
    eat();
    expect(LPAREN, "switch");
    auto expr = parseExpression();
    expect(RPAREN, "switch");
    expect(LBRACE, "switch");
    auto node = std::make_shared<Node>(NodeKind::SWITCH, "switch");
    node->cond = expr;
    while (cur().type != RBRACE && cur().type != END) {
        if (cur().type == CASE) {
            eat();
            auto val = parseExpression();
            expect(COLON, "case");
            auto caseNode = std::make_shared<Node>(NodeKind::CASE);
            caseNode->cond = val;
            while (cur().type != CASE && cur().type != DEFAULT
                   && cur().type != RBRACE && cur().type != END)
                caseNode->stmts.push_back(parseStatement());
            node->stmts.push_back(caseNode);
        } else if (cur().type == DEFAULT) {
            eat();
            expect(COLON, "default");
            auto defNode = std::make_shared<Node>(NodeKind::DEFAULT_CASE);
            while (cur().type != CASE && cur().type != DEFAULT
                   && cur().type != RBRACE && cur().type != END)
                defNode->stmts.push_back(parseStatement());
            node->stmts.push_back(defNode);
        } else break;
    }
    expect(RBRACE, "switch end");
    return node;
}

NodePtr parseReturn() {
    eat();
    auto expr = parseExpression();
    expect(SEMICOLON, "return");
    auto node = std::make_shared<Node>(NodeKind::RETURN, "return");
    node->left = expr;
    return node;
}

NodePtr parseBreak() {
    eat();
    expect(SEMICOLON, "break");
    return std::make_shared<Node>(NodeKind::BREAK, "break");
}

NodePtr parseFuncDef() {
    eat(); // 'int'
    std::string name = expect(VARIABLE, "func def").value;
    expect(LPAREN, "func def");
    auto node = std::make_shared<Node>(NodeKind::FUNC_DEF, name);
    while (cur().type != RPAREN && cur().type != END) {
        if (cur().type == INT_KW) eat();
        node->params.push_back(expect(VARIABLE, "param").value);
        if (cur().type == COMMA) eat();
    }
    expect(RPAREN, "func def");
    node->body = parseBlock();
    return node;
}

NodePtr parseAssignOrExpr() {
    if (cur().type == VARIABLE && pos+1 < (int)tokens.size()
        && tokens[pos+1].type == ASSIGN) {
        std::string name = eat().value;
        eat();
        auto val = parseExpression();
        expect(SEMICOLON, "assign");
        auto node = std::make_shared<Node>(NodeKind::ASSIGN, name);
        node->right = val;
        return node;
    }
    auto expr = parseExpression();
    expect(SEMICOLON, "expr stmt");
    auto node = std::make_shared<Node>(NodeKind::EXPR_STMT);
    node->left = expr;
    return node;
}

NodePtr parseStatement() {
    switch (cur().type) {
        case IF:        return parseIfStmt();
        case WHILE:     return parseWhileStmt();
        case DO:        return parseDoWhileStmt();
        case SWITCH:    return parseSwitchStmt();
        case RETURN_KW: return parseReturn();
        case BREAK_KW:  return parseBreak();
        case INT_KW:    return parseFuncDef();
        default:        return parseAssignOrExpr();
    }
}

NodePtr parseProgram() {
    auto prog = std::make_shared<Node>(NodeKind::BLOCK, "program");
    while (cur().type != END)
        prog->stmts.push_back(parseStatement());
    return prog;
}
