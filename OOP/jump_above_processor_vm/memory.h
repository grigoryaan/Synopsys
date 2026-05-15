#pragma once
#include "ir.h"
#include <vector>
#include <cstdint>
#include <string>

constexpr int XLEN      = 32;
constexpr int MEM_TOTAL = 512 * 1024 * 1024;

constexpr int CODE_BASE  = 0x00000000;
constexpr int CODE_SIZE  = 0x08000000;
constexpr int DATA_BASE  = 0x08000000;
constexpr int DATA_SIZE  = 0x08000000;
constexpr int STACK_BASE = 0x10000000;
constexpr int STACK_SIZE = 0x08000000;
constexpr int FREE_BASE  = 0x18000000;
constexpr int FREE_SIZE  = 0x08000000;

struct FlatInstruction {
    OpCode op;
    int    rs1, rs2, rd, offset;
    double value;
    char   varName[32];
};

struct Memory {
    std::vector<uint8_t> raw;

    Memory();

    void     writeByte(int addr, uint8_t  v);
    void     writeWord(int addr, uint32_t v);
    uint8_t  readByte (int addr) const;
    uint32_t readWord (int addr) const;

    void        writeInstruction(int slot, const Instruction& ins);
    Instruction readInstruction (int slot) const;
    int         maxCodeSlots()             const;

    void dumpLayout() const;
};
