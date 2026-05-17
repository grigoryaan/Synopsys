#include <iostream>
#include <iomanip>
#include <vector>

#include "token.h"
#include "parser.h"
#include "ast_print.h"
#include "ir.h"
#include "exec_file.h"
#include "linker.h"
#include "memory.h"
#include "loader.h"
#include "processor.h"
#include "debugger.h"

static void disassemble(const std::vector<Instruction>& code) {
    std::cout << "\n=== IR (" << code.size() << " instructions) ===\n";
    std::cout << std::left
              << std::setw(4)  << "IP"
              << std::setw(12) << "op"
              << std::setw(6)  << "rs1"
              << std::setw(6)  << "rs2"
              << std::setw(6)  << "rd"
              << std::setw(8)  << "offset"
              << "extra\n"
              << std::string(52, '-') << "\n";
    for (int i = 0; i < (int)code.size(); i++) {
        const auto& ins = code[i];
        std::cout << std::setw(4)  << i
                  << std::setw(12) << opName(ins.op)
                  << std::setw(6)  << ins.rs1
                  << std::setw(6)  << ins.rs2
                  << std::setw(6)  << ins.rd
                  << std::setw(8)  << ins.offset;
        if (ins.op == OpCode::LOAD_CONST) std::cout << "  val=" << ins.value;
        if (!ins.varName.empty())         std::cout << "  var=" << ins.varName;
        if (ins.op == OpCode::JMP  || ins.op == OpCode::JA  ||
            ins.op == OpCode::CALL || ins.op == OpCode::BRL ||
            (ins.op >= OpCode::BR_GT && ins.op <= OpCode::BR_LTE))
            std::cout << "  ->IP=" << (i + 1 + ins.offset);
        std::cout << "\n";
    }
}

int main() {
    std::string source = R"(
        int add(int a, int b) {
            return a + b;
        }

        int mul3(int a, int b, int c) {
            return a * b + c;
        }

        x = add(3, 4);
        y = mul3(2, 5, 1);
        z = add(x, y);
    )";

    std::cout << "=== Source ===\n" << source << "\n";

    tokenize(source);
    pos = 0;

    NodePtr ast = parseProgram();
    std::cout << "=== AST ===\n";
    printAST(ast);

    std::vector<Instruction> code;
    generate(ast, code);
    patchCalls(code);
    disassemble(code);

    ExecFile ef = buildExecFile(code, getFuncAddrs());
    printExecFile(ef);

    relocate(ef, 0x1000);

    Memory mem;
    mem.dumpLayout();
    int count = loadExecFile(ef, mem);

    Processor cpu(mem, count);
    cpu.run();

    std::cout << "\n=== Result ===\n";
    cpu.printState();

    return 0;
}
