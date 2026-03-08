#include <iostream>
#include <vector>
#include <map>
#include <cctype>
#include <string>
#include <set>

using namespace std;

enum TokenType {
    NUMBER,
    VARIABLE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAREN,
    RPAREN,
    END
};

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokens;
int current_index = 0;
map<string, double> variables;

void tokenize(const string& input) {
    tokens.clear();

    for (int i = 0; i < input.length(); i++) {

        char c = input[i];

        if (isspace(c))
            continue;

        // number
        if (isdigit(c)) {
            string num;

            while (i < input.length() && (isdigit(input[i]) || input[i] == '.')) {
                num += input[i];
                i++;
            }

            i--;
            tokens.push_back({ NUMBER, num });
        }

        // variable
        else if (isalpha(c)) {
            string name;

            while (i < input.length() && isalnum(input[i])) {
                name += input[i];
                i++;
            }

            i--;
            tokens.push_back({ VARIABLE, name });
        }

        // operators
        else {
            if (c == '+') tokens.push_back({ PLUS,"+" });
            else if (c == '-') tokens.push_back({ MINUS,"-" });
            else if (c == '*') tokens.push_back({ MUL,"*" });
            else if (c == '/') tokens.push_back({ DIV,"/" });
            else if (c == '(') tokens.push_back({ LPAREN,"(" });
            else if (c == ')') tokens.push_back({ RPAREN,")" });
        }
    }

    tokens.push_back({ END,"" });
}

set<string> extractVariables() {
    set<string> vars;

    for (auto& t : tokens) {
        if (t.type == VARIABLE) {
            vars.insert(t.value);
        }
    }

    return vars;
}

double parseExpression();

double parseFactor() {

    Token t = tokens[current_index++];

    if (t.type == NUMBER)
        return stod(t.value);

    if (t.type == VARIABLE)
        return variables[t.value];

    if (t.type == LPAREN) {
        double result = parseExpression();
        current_index++;
        return result;
    }

    return 0;
}

double parseTerm() {

    double result = parseFactor();

    while (tokens[current_index].type == MUL || tokens[current_index].type == DIV) {

        Token op = tokens[current_index++];

        if (op.type == MUL)
            result *= parseFactor();
        else
            result /= parseFactor();
    }

    return result;
}

double parseExpression() {

    double result = parseTerm();

    while (tokens[current_index].type == PLUS || tokens[current_index].type == MINUS) {

        Token op = tokens[current_index++];

        if (op.type == PLUS)
            result += parseTerm();
        else
            result -= parseTerm();
    }

    return result;
}

int main() {

    string input;

    cout << "Enter expression: ";
    getline(cin, input);

    tokenize(input);

    set<string> vars = extractVariables();

    for (double x = 0; x <= 100; x += 0.01) {

        for (auto& v : vars) {
            variables[v] = x;
        }

        current_index = 0;

        double result = parseExpression();

        cout << "value = " << x << "  result = " << result << endl;
    }

    return 0;
}
