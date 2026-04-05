#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "token.h"
#include "parser.h"
#include "ast_print.h"
#include "ir.h"

int main() {

    std::string source = R"(
        x = 1;
        result = 0;
        while (x <= 10) {
            if (x == 5) {
                result = result + 100;
            } else {
                result = result + x;
            }
            x = x + 1;
        }
    )";

    std::cout << "=== Source ===\n" << source << "\n";

    tokenize(source);
    pos = 0;

    NodePtr ast = parseProgram();

    std::cout << "=== AST ===\n";
    printAST(ast);

    std::vector<Instruction> code;
    generate(ast, code);

    std::cout << "\n=== Instruction Vector (" << code.size() << " instructions) ===\n";
    for (int i = 0; i < (int)code.size(); i++) {
        std::cout << i << "\top=" << (int)code[i].op
                  << " left="  << code[i].left
                  << " right=" << code[i].right
                  << " dest="  << code[i].dest;
        if (!code[i].varName.empty())
            std::cout << " var=" << code[i].varName;
        if (code[i].op == OpCode::LOAD_CONST)
            std::cout << " val=" << code[i].value;
        std::cout << "\n";
    }

    // ── Execute ───────────────────────────────────────────────
    std::map<std::string, double> vars;
    execute(code, vars);

    std::cout << "\n=== Variables after execution ===\n";
    for (auto& [k, v] : vars)
        std::cout << k << " = " << v << "\n";

    return 0;
}
