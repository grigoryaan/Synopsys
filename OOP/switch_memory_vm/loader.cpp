#include "loader.h"
#include <stdexcept>
#include <iostream>

int load(const std::vector<Instruction>& code, Memory& mem) {
    if ((int)code.size() > mem.maxCodeSlots())
        throw std::runtime_error("Program too large for code segment");
    for (int i = 0; i < (int)code.size(); i++)
        mem.writeInstruction(i, code[i]);
    std::cout << "[Loader] " << code.size() << " instructions loaded into code segment\n";
    return (int)code.size();
}

double executeFromMemory(Memory& mem, int instrCount,
                         std::map<std::string,double>& vars) {

    std::vector<double> reg(instrCount, 0.0);
    int pc = 0;

    while (pc < instrCount) {
        Instruction ins = mem.readInstruction(pc);

        switch (ins.op) {
        case OpCode::LOAD_CONST: reg[ins.rd] = ins.value; break;
        case OpCode::LOAD_VAR:   reg[ins.rd] = vars.count(ins.varName) ? vars[ins.varName] : 0.0; break;
        case OpCode::STORE_VAR:  vars[ins.varName] = reg[ins.rs1]; reg[ins.rd] = reg[ins.rs1]; break;
        case OpCode::ADD:  reg[ins.rd] = reg[ins.rs1] + reg[ins.rs2]; break;
        case OpCode::SUB:  reg[ins.rd] = reg[ins.rs1] - reg[ins.rs2]; break;
        case OpCode::MUL:  reg[ins.rd] = reg[ins.rs1] * reg[ins.rs2]; break;
        case OpCode::DIV:  reg[ins.rd] = reg[ins.rs1] / reg[ins.rs2]; break;
        case OpCode::CMP_GT:  reg[ins.rd] = reg[ins.rs1] >  reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_LT:  reg[ins.rd] = reg[ins.rs1] <  reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_EQ:  reg[ins.rd] = reg[ins.rs1] == reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_NEQ: reg[ins.rd] = reg[ins.rs1] != reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_GTE: reg[ins.rd] = reg[ins.rs1] >= reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_LTE: reg[ins.rd] = reg[ins.rs1] <= reg[ins.rs2] ? 1:0; break;
        case OpCode::JMP: pc = pc + 1 + ins.offset; continue;
        case OpCode::BR_GT:  if (reg[ins.rs1] >  reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_LT:  if (reg[ins.rs1] <  reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_EQ:  if (reg[ins.rs1] == reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_NEQ: if (reg[ins.rs1] != reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_GTE: if (reg[ins.rs1] >= reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_LTE: if (reg[ins.rs1] <= reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::NOP: break;
        }
        ++pc;
    }
    return reg.empty() ? 0.0 : reg.back();
}
