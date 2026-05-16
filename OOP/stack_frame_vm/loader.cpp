#include "loader.h"
#include <stdexcept>
#include <iostream>
#include <map>

int load(const std::vector<Instruction>& code, Memory& mem) {
    if ((int)code.size() > mem.maxCodeSlots())
        throw std::runtime_error("Program too large for code segment");
    for (int i = 0; i < (int)code.size(); i++)
        mem.writeInstruction(i, code[i]);
    std::cout << "[Loader] " << code.size()
              << " instructions -> code segment (0x0000)\n";
    return (int)code.size();
}
