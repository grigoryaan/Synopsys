#include "debugger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Debugger::Debugger(Memory& m, int count, std::map<std::string,double>& v)
    : mem(m), instrCount(count), vars(v), reg(count, 0.0) {}

void Debugger::addBreakpoint(int idx)    { breakpoints.insert(idx); }
void Debugger::removeBreakpoint(int idx) { breakpoints.erase(idx);  }

bool Debugger::executeOne() {
    if (pc < 0 || pc >= instrCount) { done = true; return false; }
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
    case OpCode::JMP: pc = pc + 1 + ins.offset; return true;
    case OpCode::BR_GT:  if (reg[ins.rs1] >  reg[ins.rs2]) { pc=pc+1+ins.offset; return true; } break;
    case OpCode::BR_LT:  if (reg[ins.rs1] <  reg[ins.rs2]) { pc=pc+1+ins.offset; return true; } break;
    case OpCode::BR_EQ:  if (reg[ins.rs1] == reg[ins.rs2]) { pc=pc+1+ins.offset; return true; } break;
    case OpCode::BR_NEQ: if (reg[ins.rs1] != reg[ins.rs2]) { pc=pc+1+ins.offset; return true; } break;
    case OpCode::BR_GTE: if (reg[ins.rs1] >= reg[ins.rs2]) { pc=pc+1+ins.offset; return true; } break;
    case OpCode::BR_LTE: if (reg[ins.rs1] <= reg[ins.rs2]) { pc=pc+1+ins.offset; return true; } break;
    case OpCode::NOP: break;
    }
    ++pc;
    if (pc >= instrCount) done = true;
    return !done;
}

bool Debugger::step() {
    if (done) return false;
    printState();
    return executeOne();
}

void Debugger::cont() {
    while (!done) {
        if (breakpoints.count(pc)) {
            std::cout << "[Debugger] Breakpoint hit at instruction " << pc << "\n";
            printState();
            return;
        }
        executeOne();
    }
    std::cout << "[Debugger] Program finished\n";
}

void Debugger::printState() const {
    Instruction ins = mem.readInstruction(pc);
    std::cout << "\n[pc=" << std::setw(3) << pc << "] "
              << std::left << std::setw(12) << opName(ins.op)
              << " rs1=" << ins.rs1 << " rs2=" << ins.rs2
              << " rd=" << ins.rd << " off=" << ins.offset;
    if (!ins.varName.empty()) std::cout << " var=" << ins.varName;
    if (ins.op == OpCode::LOAD_CONST) std::cout << " val=" << ins.value;
    std::cout << "\n";

    if (!vars.empty()) {
        std::cout << "  vars: ";
        for (auto& [k,v] : vars) std::cout << k << "=" << v << " ";
        std::cout << "\n";
    }
}

void Debugger::runREPL() {
    std::cout << "Debugger started. Commands: s=step  c=continue  b <n>=breakpoint  r=remove bp  q=quit\n";
    std::string line;
    while (!done && std::getline(std::cin, line)) {
        if (line.empty() || line == "s") {
            step();
        } else if (line == "c") {
            cont();
        } else if (line.rfind("b ", 0) == 0) {
            int n = std::stoi(line.substr(2));
            addBreakpoint(n);
            std::cout << "Breakpoint set at " << n << "\n";
        } else if (line.rfind("r ", 0) == 0) {
            int n = std::stoi(line.substr(2));
            removeBreakpoint(n);
            std::cout << "Breakpoint removed from " << n << "\n";
        } else if (line == "q") {
            break;
        } else {
            std::cout << "Unknown command\n";
        }
        if (done) std::cout << "[Debugger] Program finished\n";
    }
}
