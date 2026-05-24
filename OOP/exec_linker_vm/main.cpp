#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "token.h"
#include "parser.h"
#include "ir.h"
#include "exec_file.h"
#include "linker.h"
#include "memory.h"
#include "processor.h"

static void runProgram(const std::string& source, bool verbose) {
    try {
        tokenize(source);
        pos = 0;
        NodePtr ast = parseProgram();

        std::vector<Instruction> code;
        generate(ast, code);
        ExecFile ef = buildExecFile(code, getFuncAddrs());
        patchCalls(code);
        ef.code = code;
        relocate(ef, 0x0000);

        Memory mem;
        int count = loadExecFile(ef, mem);

        Processor cpu(mem, count);
        cpu.run();

        if (verbose && !ef.symbolTable.empty()) {
            std::cout << "[Symbol Table]\n";
            for (auto& s : ef.symbolTable)
                std::cout << "  " << s.name << " @ " << s.offset << " (" << s.type << ")\n";
            std::cout << "[Jump Table]\n";
            for (auto& j : ef.jumpTable)
                std::cout << "  " << j.name << " -> 0x" << std::hex << j.offset << std::dec << "\n";
        }

        std::cout << "[Output]\n";
        bool any = false;
        for (auto it2 = cpu.vars.begin(); it2 != cpu.vars.end(); ++it2) {
            if (it2->first[0] == '_') continue;
            std::cout << "  " << it2->first << " = " << it2->second << "\n";
            any = true;
        }
        if (!any) std::cout << "  (no variables)\n";

    } catch (std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
    }
}

static void printHelp() {
    std::cout <<
        "Usage: type code, end block with '.' on its own line.\n\n"
        "Examples:\n"
        "  x = 5; y = x + 3;\n"
        "  .\n\n"
        "  switch (x) {\n"
        "    case 1: y = 10; break;\n"
        "    case 5: y = 50; break;\n"
        "    default: y = 0;\n"
        "  }\n"
        "  .\n\n"
        "  int add(int a, int b) { return a + b; }\n"
        "  result = add(3, 4);\n"
        "  .\n\n"
        "Commands: help, verbose, quit\n";
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::ifstream f(argv[1]);
        if (!f) { std::cerr << "Cannot open: " << argv[1] << "\n"; return 1; }
        std::ostringstream ss; ss << f.rdbuf();
        runProgram(ss.str(), true);
        return 0;
    }

    std::cout << "=== Mini Compiler ===\n"
              << "Enter code, end with '.' on its own line. Type 'help' for examples.\n\n";

    bool verbose = false;
    std::string line, source;

    while (true) {
        if (source.empty()) std::cout << ">> ";
        else                std::cout << ".. ";
        std::cout.flush();

        if (!std::getline(std::cin, line)) {
            if (!source.empty()) runProgram(source, verbose);
            break;
        }

        if (source.empty()) {
            if (line == "quit" || line == "q" || line == "exit") break;
            if (line == "help")    { printHelp(); continue; }
            if (line == "verbose") {
                verbose = !verbose;
                std::cout << "[verbose " << (verbose ? "on" : "off") << "]\n";
                continue;
            }
            if (line.empty()) continue;
        }

        if (line == ".") {
            if (!source.empty()) {
                runProgram(source, verbose);
                source.clear();
            }
            continue;
        }

        source += line + "\n";
    }

    return 0;
}
