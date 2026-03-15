#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <set>

using namespace std;

enum TokenType {
    NUMBER, VARIABLE,
    PLUS, MINUS, MUL, DIV,
    LPAREN, RPAREN,
    END
};

struct Token {
    TokenType type;
    double value;
    string name;
};

vector<Token> tokens;
int pos = 0;

set<string> variables;

void tokenize(string input) {

    tokens.clear();
    variables.clear();

    for (int i = 0; i < input.size(); i++) {

        if (isspace(input[i]))
            continue;

        if (isdigit(input[i])) {

            string num;

            while (i < input.size() && (isdigit(input[i]) || input[i]=='.'))
                num += input[i++];

            i--;

            tokens.push_back({NUMBER, stod(num)});
        }

        else if (isalpha(input[i])) {

            string name;

            while (i < input.size() && isalpha(input[i]))
                name += input[i++];

            i--;

            variables.insert(name);

            tokens.push_back({VARIABLE,0,name});
        }

        else {

            char c = input[i];

            if (c=='+') tokens.push_back({PLUS});
            if (c=='-') tokens.push_back({MINUS});
            if (c=='*') tokens.push_back({MUL});
            if (c=='/') tokens.push_back({DIV});
            if (c=='(') tokens.push_back({LPAREN});
            if (c==')') tokens.push_back({RPAREN});
        }
    }

    tokens.push_back({END});
}

struct Node {

    TokenType type;
    double value;
    string name;

    Node* left;
    Node* right;

    Node(TokenType t,double v=0,string n="") {
        type=t;
        value=v;
        name=n;
        left=nullptr;
        right=nullptr;
    }
};

Token current() {
    return tokens[pos];
}

void eat(TokenType type) {
    if (current().type == type)
        pos++;
}

Node* parseExpression();

Node* parseFactor() {

    Token t = current();

    if (t.type == NUMBER) {
        eat(NUMBER);
        return new Node(NUMBER,t.value);
    }

    if (t.type == VARIABLE) {
        eat(VARIABLE);
        return new Node(VARIABLE,0,t.name);
    }

    if (t.type == LPAREN) {
        eat(LPAREN);
        Node* node = parseExpression();
        eat(RPAREN);
        return node;
    }

    return nullptr;
}

Node* parseTerm() {

    Node* node = parseFactor();

    while (current().type==MUL || current().type==DIV) {

        TokenType op=current().type;
        eat(op);

        Node* newNode = new Node(op);
        newNode->left = node;
        newNode->right = parseFactor();

        node = newNode;
    }

    return node;
}

Node* parseExpression() {

    Node* node = parseTerm();

    while (current().type==PLUS || current().type==MINUS) {

        TokenType op=current().type;
        eat(op);

        Node* newNode = new Node(op);
        newNode->left = node;
        newNode->right = parseTerm();

        node = newNode;
    }

    return node;
}

double evaluate(Node* node,double value) {

    if (node->type == NUMBER)
        return node->value;

    if (node->type == VARIABLE)
        return value;

    double left = evaluate(node->left,value);
    double right = evaluate(node->right,value);

    if (node->type==PLUS) return left+right;
    if (node->type==MINUS) return left-right;
    if (node->type==MUL) return left*right;
    if (node->type==DIV) return left/right;

    return 0;
}

int main() {

    string expr;

    cout<<"Enter expression: ";
    getline(cin,expr);

    tokenize(expr);

    pos = 0;

    Node* tree = parseExpression();

    cout<<"Variables found: ";

    for(auto &v : variables)
        cout<<v<<" ";

    cout<<endl<<endl;

    for(double x=1; x<=100; x+=0.1) {

        double result = evaluate(tree,x);

        cout<<"value="<<x<<" result="<<result<<endl;
    }

    return 0;
}
