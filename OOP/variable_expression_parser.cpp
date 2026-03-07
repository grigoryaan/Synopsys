#include <iostream>
#include <string>
#include <map>
#include <set>
#include <cmath>
using namespace std;

class Parser {
private:
    string expr;
    int pos;
    map<string, double>& variables;

    void skipSpaces() {
        while (pos < expr.size() && expr[pos] == ' ')
            pos++;
    }

    string parseVariable() {
        skipSpaces();
        string name;
        while (pos < expr.size() && isalpha(expr[pos])) {
            name += expr[pos++];
        }
        return name;
    }

    double parseNumber() {
        skipSpaces();
        double number = 0;

        while (pos < expr.size() && isdigit(expr[pos])) {
            number = number * 10 + (expr[pos] - '0');
            pos++;
        }

        if (pos < expr.size() && expr[pos] == '.') {
            pos++;
            double frac = 0;
            double base = 0.1;

            while (pos < expr.size() && isdigit(expr[pos])) {
                frac += (expr[pos] - '0') * base;
                base *= 0.1;
                pos++;
            }
            number += frac;
        }

        return number;
    }

    double parseFactor() {
        skipSpaces();

        if (expr[pos] == '(') {
            pos++;
            double result = parseExpression();
            pos++;
            return result;
        }

        if (isalpha(expr[pos])) {
            string var = parseVariable();
            return variables[var];
        }

        return parseNumber();
    }

    double parseTerm() {
        double result = parseFactor();

        while (true) {
            skipSpaces();

            if (expr[pos] == '*') {
                pos++;
                result *= parseFactor();
            }
            else if (expr[pos] == '/') {
                pos++;
                result /= parseFactor();
            }
            else break;
        }

        return result;
    }

    double parseExpression() {
        double result = parseTerm();

        while (true) {
            skipSpaces();

            if (expr[pos] == '+') {
                pos++;
                result += parseTerm();
            }
            else if (expr[pos] == '-') {
                pos++;
                result -= parseTerm();
            }
            else break;
        }

        return result;
    }

public:
    Parser(string s, map<string, double>& vars)
        : expr(s), pos(0), variables(vars) {}

    double calculate() {
        pos = 0;
        return parseExpression();
    }
};


set<string> extractVariables(const string& expr) {
    set<string> vars;
    string name;

    for (char c : expr) {
        if (isalpha(c)) {
            name += c;
        } else {
            if (!name.empty()) {
                vars.insert(name);
                name.clear();
            }
        }
    }

    if (!name.empty())
        vars.insert(name);

    return vars;
}

int main() {
    string input;
    cout << "Enter expression: ";
    getline(cin, input);

    set<string> varNames = extractVariables(input);

    map<string, double> variables;

    for (double value = 1; value <= 100; value += 0.1) {

        for (auto& v : varNames)
            variables[v] = value;

        Parser parser(input, variables);

        double result = parser.calculate();

        cout << "Value = " << value
             << " Result = " << result << endl;
    }

    return 0;
}
