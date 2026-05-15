#include "processor.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

Processor::Processor(Memory& m, int count)
    : IP(0), instrCount(count), mem(m), freg(count, 0.0), jumped(false) {
    for (int i = 0; i < NUM_REGS; i++) regs[i] = 0;
}

void Processor::fetch() {
    IR = mem.readInstruction(IP);
}

void Processor::decode() {
    if (IR.rd >= instrCount && IR.rd != -1)
        throw std::runtime_error("decode: rd out of range");
    jumped = false;
    ++IP;
}

void Processor::execute() {
    switch (IR.op) {
    case OpCode::LOAD_CONST:
        freg[IR.rd] = IR.value;
        break;
    case OpCode::LOAD_VAR:
        freg[IR.rd] = vars.count(IR.varName) ? vars[IR.varName] : 0.0;
        break;
    case OpCode::STORE_VAR:
        vars[IR.varName] = freg[IR.rs1];
        freg[IR.rd]      = freg[IR.rs1];
        regs[IR.rd % NUM_REGS] = (uint32_t)(int32_t)freg[IR.rd];
        break;
    case OpCode::ADD: freg[IR.rd] = freg[IR.rs1] + freg[IR.rs2]; break;
    case OpCode::SUB: freg[IR.rd] = freg[IR.rs1] - freg[IR.rs2]; break;
    case OpCode::MUL: freg[IR.rd] = freg[IR.rs1] * freg[IR.rs2]; break;
    case OpCode::DIV: freg[IR.rd] = freg[IR.rs1] / freg[IR.rs2]; break;
    case OpCode::CMP_GT:  freg[IR.rd] = freg[IR.rs1] >  freg[IR.rs2] ? 1:0; break;
    case OpCode::CMP_LT:  freg[IR.rd] = freg[IR.rs1] <  freg[IR.rs2] ? 1:0; break;
    case OpCode::CMP_EQ:  freg[IR.rd] = freg[IR.rs1] == freg[IR.rs2] ? 1:0; break;
    case OpCode::CMP_NEQ: freg[IR.rd] = freg[IR.rs1] != freg[IR.rs2] ? 1:0; break;
    case OpCode::CMP_GTE: freg[IR.rd] = freg[IR.rs1] >= freg[IR.rs2] ? 1:0; break;
    case OpCode::CMP_LTE: freg[IR.rd] = freg[IR.rs1] <= freg[IR.rs2] ? 1:0; break;

    case OpCode::JMP:
        IP = IP + IR.offset;
        jumped = true;
        break;

    case OpCode::JA: {
        uint32_t a = (uint32_t)(int32_t)freg[IR.rs1];
        uint32_t b = (uint32_t)(int32_t)freg[IR.rs2];
        if (a > b) { IP = IP + IR.offset; jumped = true; }
        break;
    }

    case OpCode::BR_GT:  if (freg[IR.rs1] >  freg[IR.rs2]) { IP=IP+IR.offset; jumped=true; } break;
    case OpCode::BR_LT:  if (freg[IR.rs1] <  freg[IR.rs2]) { IP=IP+IR.offset; jumped=true; } break;
    case OpCode::BR_EQ:  if (freg[IR.rs1] == freg[IR.rs2]) { IP=IP+IR.offset; jumped=true; } break;
    case OpCode::BR_NEQ: if (freg[IR.rs1] != freg[IR.rs2]) { IP=IP+IR.offset; jumped=true; } break;
    case OpCode::BR_GTE: if (freg[IR.rs1] >= freg[IR.rs2]) { IP=IP+IR.offset; jumped=true; } break;
    case OpCode::BR_LTE: if (freg[IR.rs1] <= freg[IR.rs2]) { IP=IP+IR.offset; jumped=true; } break;

    case OpCode::NOP: break;
    }
}

bool Processor::step() {
    if (IP < 0 || IP >= instrCount) return false;
    fetch();
    decode();
    execute();
    return IP >= 0 && IP < instrCount;
}

void Processor::run() {
    while (IP >= 0 && IP < instrCount) {
        fetch();
        decode();
        execute();
    }
}

void Processor::printState() const {
    std::cout << "IP=" << std::setw(3) << IP
              << "  IR=[" << opName(IR.op) << "]";
    if (!IR.varName.empty()) std::cout << " " << IR.varName;
    std::cout << "\n  regs: ";
    for (int i = 0; i < NUM_REGS; i++)
        if (regs[i]) std::cout << "R" << i << "=" << regs[i] << " ";
    std::cout << "\n  vars: ";
    for (auto& [k,v] : vars) std::cout << k << "=" << v << " ";
    std::cout << "\n";
}
