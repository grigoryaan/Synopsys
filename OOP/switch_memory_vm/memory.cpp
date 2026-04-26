#include "memory.h"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <iomanip>


struct FlatInstruction {
    OpCode op;
    int    rs1, rs2, rd, offset;
    double value;
    char   varName[32]; 
};

static FlatInstruction toFlat(const Instruction& ins) {
    FlatInstruction f{};
    f.op     = ins.op;
    f.rs1    = ins.rs1;  f.rs2 = ins.rs2;
    f.rd     = ins.rd;   f.offset = ins.offset;
    f.value  = ins.value;
    std::strncpy(f.varName, ins.varName.c_str(), sizeof(f.varName)-1);
    return f;
}

static Instruction fromFlat(const FlatInstruction& f) {
    Instruction ins;
    ins.op      = f.op;
    ins.rs1     = f.rs1; ins.rs2 = f.rs2;
    ins.rd      = f.rd;  ins.offset = f.offset;
    ins.value   = f.value;
    ins.varName = f.varName;
    return ins;
}

Memory::Memory() : raw(TOTAL, 0) {}

void Memory::writeByte(int addr, uint8_t v) {
    if (addr < 0 || addr >= TOTAL) throw std::runtime_error("Memory write OOB");
    raw[addr] = v;
}

void Memory::writeWord(int addr, int32_t v) {
    if (addr + (int)sizeof(v) > TOTAL) throw std::runtime_error("Memory write OOB");
    std::memcpy(raw.data() + addr, &v, sizeof(v));
}

uint8_t Memory::readByte(int addr) const {
    if (addr < 0 || addr >= TOTAL) throw std::runtime_error("Memory read OOB");
    return raw[addr];
}

int32_t Memory::readWord(int addr) const {
    if (addr + (int)sizeof(int32_t) > TOTAL) throw std::runtime_error("Memory read OOB");
    int32_t v; std::memcpy(&v, raw.data() + addr, sizeof(v));
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

void Memory::dumpSegment(const std::string& name, int base, int size) const {
    std::cout << "-- " << name << " (base=" << base << ", size=" << size << ") --\n";
    for (int i = 0; i < size; i += 16) {
        bool anyNonZero = false;
        for (int j = i; j < i+16 && j < size; j++)
            if (raw[base+j]) { anyNonZero = true; break; }
        if (!anyNonZero) continue;
        std::cout << std::hex << std::setw(4) << std::setfill('0') << (base+i) << ": ";
        for (int j = i; j < i+16 && j < size; j++)
            std::cout << std::setw(2) << (int)raw[base+j] << " ";
        std::cout << std::dec << "\n";
    }
}
