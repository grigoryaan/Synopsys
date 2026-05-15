#include "memory.h"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <iomanip>

static FlatInstruction toFlat(const Instruction& ins) {
    FlatInstruction f{};
    f.op = ins.op; f.rs1 = ins.rs1; f.rs2 = ins.rs2;
    f.rd = ins.rd; f.offset = ins.offset; f.value = ins.value;
    std::strncpy(f.varName, ins.varName.c_str(), sizeof(f.varName)-1);
    return f;
}

static Instruction fromFlat(const FlatInstruction& f) {
    Instruction ins;
    ins.op = f.op; ins.rs1 = f.rs1; ins.rs2 = f.rs2;
    ins.rd = f.rd; ins.offset = f.offset; ins.value = f.value;
    ins.varName = f.varName;
    return ins;
}

Memory::Memory() : raw(MEM_TOTAL, 0) {}

void Memory::writeByte(int addr, uint8_t v) {
    if (addr < 0 || addr >= MEM_TOTAL) throw std::runtime_error("Memory OOB write");
    raw[addr] = v;
}

void Memory::writeWord(int addr, uint32_t v) {
    if (addr < 0 || addr + 3 >= MEM_TOTAL) throw std::runtime_error("Memory OOB write");
    std::memcpy(raw.data() + addr, &v, sizeof(v));
}

uint8_t Memory::readByte(int addr) const {
    if (addr < 0 || addr >= MEM_TOTAL) throw std::runtime_error("Memory OOB read");
    return raw[addr];
}

uint32_t Memory::readWord(int addr) const {
    if (addr < 0 || addr + 3 >= MEM_TOTAL) throw std::runtime_error("Memory OOB read");
    uint32_t v; std::memcpy(&v, raw.data() + addr, sizeof(v));
    return v;
}

void Memory::writeInstruction(int slot, const Instruction& ins) {
    int addr = CODE_BASE + slot * (int)sizeof(FlatInstruction);
    if (addr + (int)sizeof(FlatInstruction) > CODE_BASE + CODE_SIZE)
        throw std::runtime_error("Code segment full");
    FlatInstruction f = toFlat(ins);
    std::memcpy(raw.data() + addr, &f, sizeof(f));
}

Instruction Memory::readInstruction(int slot) const {
    int addr = CODE_BASE + slot * (int)sizeof(FlatInstruction);
    FlatInstruction f;
    std::memcpy(&f, raw.data() + addr, sizeof(f));
    return fromFlat(f);
}

int Memory::maxCodeSlots() const { return CODE_SIZE / (int)sizeof(FlatInstruction); }

void Memory::dumpLayout() const {
    std::cout << "Memory layout (64K, XLEN=" << XLEN << "):\n"
              << "  0x" << std::hex << std::setfill('0')
              << std::setw(4) << CODE_BASE  << " - 0x" << std::setw(4) << (CODE_BASE  + CODE_SIZE  - 1) << "  Code\n"
              << "  0x" << std::setw(4) << DATA_BASE  << " - 0x" << std::setw(4) << (DATA_BASE  + DATA_SIZE  - 1) << "  Data\n"
              << "  0x" << std::setw(4) << STACK_BASE << " - 0x" << std::setw(4) << (STACK_BASE + STACK_SIZE - 1) << "  Stack\n"
              << std::dec;
}
