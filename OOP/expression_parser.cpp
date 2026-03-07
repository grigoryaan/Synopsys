#include <iostream>
#include <string>
using namespace std;

class Parser {
private:
    string expr;
    int pos;

    void skipSpaces() {
        while (pos < expr.size() && expr[pos] == ' ')
            pos++;
    }

    double parseNumber() {
        skipSpaces();
        double number = 0;
        while (pos < expr.size() && isdigit(expr[pos])) {
            number = number * 10 + (expr[pos] - '0');
            pos++;
        }
        return number;
    }

    double parseFactor() {
        skipSpaces();

        if (expr[pos] == '(') {
            pos++; // пропускаем '('
            double result = parseExpression();
            pos++; // пропускаем ')'
            return result;
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
                double divisor = parseFactor();
                if (divisor == 0) {
                    throw runtime_error("Division by zero!");
                }
                result /= divisor;
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
    Parser(string s) : expr(s), pos(0) {}

    double calculate() {
        return parseExpression();
    }
};

int main() {
    string input;
    cout << "Enter expression: ";
    getline(cin, input);

    try {
        Parser parser(input);
        cout << "Result: " << parser.calculate() << endl;
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
