#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

#include "token.h"
#include "parser.h"
#include "ast_print.h"
#include "ir.h"

static void disassemble(const std::vector<Instruction>& code) {
    std::cout << "\n=== Instruction Vector (" << code.size() << " instructions) ===\n";
    std::cout << std::left
              << std::setw(4)  << "idx"
              << std::setw(12) << "op"
              << std::setw(6)  << "rs1"
              << std::setw(6)  << "rs2"
              << std::setw(6)  << "rd"
              << std::setw(10) << "offset"
              << "extra\n";
    std::cout << std::string(54, '-') << "\n";

    for (int i = 0; i < (int)code.size(); i++) {
        const auto& ins = code[i];
        std::cout << std::setw(4)  << i
                  << std::setw(12) << opName(ins.op)
                  << std::setw(6)  << ins.rs1
                  << std::setw(6)  << ins.rs2
                  << std::setw(6)  << ins.rd
                  << std::setw(10) << ins.offset;

        if (ins.op == OpCode::LOAD_CONST)
            std::cout << "  val=" << ins.value;
        if (!ins.varName.empty())
            std::cout << "  var=" << ins.varName;

        if (ins.op == OpCode::JMP || (ins.op >= OpCode::BR_GT && ins.op <= OpCode::BR_LTE))
            std::cout << "  → instr " << (i + 1 + ins.offset);

        std::cout << "\n";
    }
}

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
    disassemble(code);

    std::map<std::string, double> vars;
    execute(code, vars);

    std::cout << "\n=== Variables after execution ===\n";
    for (auto& [k, v] : vars)
        std::cout << "  " << k << " = " << v << "\n";

    return 0;
}
