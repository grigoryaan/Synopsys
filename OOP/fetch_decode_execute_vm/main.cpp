#include <iostream>
#include <iomanip>
#include <vector>

#include "token.h"
#include "parser.h"
#include "ast_print.h"
#include "ir.h"
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
              << std::string(50, '-') << "\n";
    for (int i = 0; i < (int)code.size(); i++) {
        const auto& ins = code[i];
        std::cout << std::setw(4) << i
                  << std::setw(12) << opName(ins.op)
                  << std::setw(6) << ins.rs1
                  << std::setw(6) << ins.rs2
                  << std::setw(6) << ins.rd
                  << std::setw(8) << ins.offset;
        if (ins.op == OpCode::LOAD_CONST)        std::cout << "  val=" << ins.value;
        if (!ins.varName.empty())                 std::cout << "  var=" << ins.varName;
        if (ins.op == OpCode::JMP ||
            (ins.op >= OpCode::BR_GT && ins.op <= OpCode::BR_LTE))
            std::cout << "  →IP=" << (i + 1 + ins.offset);
        std::cout << "\n";
    }
}

int main() {
    std::string source = R"(
        x = 1;
        result = 0;

        while (x <= 5) {
            switch (x) {
                case 1: result = result + 10; break;
                case 3: result = result + 30; break;
                default: result = result + 1;
            }
            x = x + 1;
        }

        y = 0;
        do {
            y = y + 1;
        } while (y < 3);
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

    Memory mem;
    mem.dumpLayout();
    int count = load(code, mem);

    Processor cpu(mem, count);
    cpu.run();

    std::cout << "\n=== Processor state after execution ===\n";
    cpu.printState();

    std::cout << "\n=== Debugger demo ===\n";
    std::string dbgSrc = "a = 2; b = a + 3; c = b * 2;";
    tokenize(dbgSrc); pos = 0;
    std::vector<Instruction> dbgCode;
    generate(parseProgram(), dbgCode);
    Memory dbgMem;
    int dbgCount = load(dbgCode, dbgMem);
    Processor dbgCpu(dbgMem, dbgCount);
    Debugger dbg(dbgCpu);
    dbg.addBreakpoint(4);
    dbg.cont();
    dbg.cont();

    return 0;
}
