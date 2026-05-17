#include "exec_file.h"
#include <iostream>
#include <iomanip>

ExecFile buildExecFile(const std::vector<Instruction>& code,
                       const std::map<std::string,int>& funcAddrs) {
    ExecFile ef;
    ef.code = code;

    for (auto& [name, offset] : funcAddrs) {
        ef.symbolTable.push_back({name, offset, "function"});
        ef.jumpTable.push_back({name, offset});
    }

    for (int i = 0; i < (int)code.size(); i++) {
        if (code[i].op == OpCode::CALL && !code[i].varName.empty()) {
            ef.relocTable.push_back({i, code[i].varName});
        }
    }

    return ef;
}

void printExecFile(const ExecFile& ef) {
    std::cout << "\n=== Exec File ===\n";
    std::cout << "Base: 0x" << std::hex << ef.baseAddress << std::dec << "\n";

    std::cout << "\n-- Symbol Table --\n";
    std::cout << std::left << std::setw(16) << "Name"
              << std::setw(10) << "Offset"
              << "Type\n";
    for (auto& s : ef.symbolTable)
        std::cout << std::setw(16) << s.name
                  << std::setw(10) << s.offset
                  << s.type << "\n";

    std::cout << "\n-- Jump Table --\n";
    std::cout << std::left << std::setw(16) << "Name"
              << "Abs Address\n";
    for (auto& j : ef.jumpTable)
        std::cout << std::setw(16) << j.name
                  << "0x" << std::hex << (ef.baseAddress + j.offset)
                  << std::dec << "\n";

    std::cout << "\n-- Reloc Table --\n";
    std::cout << std::left << std::setw(8) << "InstrIdx"
              << "Symbol\n";
    for (auto& r : ef.relocTable)
        std::cout << std::setw(8) << r.instrIndex
                  << r.symbolName << "\n";

    std::cout << "\n-- Code Section (" << ef.code.size() << " instructions) --\n";
}
