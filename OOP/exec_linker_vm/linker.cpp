#include "linker.h"
#include <iostream>

void relocate(ExecFile& ef, int baseAddress) {
    ef.baseAddress = baseAddress;

    for (auto& j : ef.jumpTable)
        j.offset = baseAddress + j.offset;

    for (auto& s : ef.symbolTable)
        s.offset = baseAddress + s.offset;

    for (auto& r : ef.relocTable) {
        for (auto& j : ef.jumpTable) {
            if (j.name == r.symbolName) {
                ef.code[r.instrIndex].offset = j.offset - baseAddress;
                break;
            }
        }
    }

    std::cout << "[Linker] Relocated to base 0x"
              << std::hex << baseAddress << std::dec << "\n";
}

int loadExecFile(ExecFile& ef, Memory& mem) {
    if ((int)ef.code.size() > mem.maxCodeSlots())
        throw std::runtime_error("Exec file too large for code segment");
    for (int i = 0; i < (int)ef.code.size(); i++)
        mem.writeInstruction(i, ef.code[i]);
    std::cout << "[Linker] Loaded " << ef.code.size()
              << " instructions at base 0x"
              << std::hex << ef.baseAddress << std::dec << "\n";
    return (int)ef.code.size();
}
