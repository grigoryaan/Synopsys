#pragma once
#include "ir.h"
#include <vector>
#include <cstdint>
#include <string>

constexpr int XLEN = 32;


struct Memory {
    static constexpr int CODE_SIZE  = 4096;
    static constexpr int DATA_SIZE  = 4096;
    static constexpr int STACK_SIZE = 4096;
    static constexpr int HEAP_SIZE  = 4096;
    static constexpr int TOTAL      = CODE_SIZE + DATA_SIZE + STACK_SIZE + HEAP_SIZE;

    static constexpr int CODE_BASE  = 0;
    static constexpr int DATA_BASE  = CODE_BASE  + CODE_SIZE;
    static constexpr int STACK_BASE = DATA_BASE  + DATA_SIZE;
    static constexpr int HEAP_BASE  = STACK_BASE + STACK_SIZE;

    std::vector<uint8_t> raw;

    Memory();

    void  writeByte(int addr, uint8_t  v);
    void  writeWord(int addr, int32_t  v);
    uint8_t  readByte(int addr) const;
    int32_t  readWord(int addr) const;

    void  writeInstruction(int slot, const Instruction& ins);
    Instruction readInstruction(int slot) const;

    int maxCodeSlots() const;

    void dumpSegment(const std::string& name, int base, int size) const;
};
