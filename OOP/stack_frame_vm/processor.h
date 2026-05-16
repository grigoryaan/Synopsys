#pragma once
#include "memory.h"
#include "ir.h"
#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct Processor {
    static constexpr int NUM_REGS = 16;

    uint32_t    regs[NUM_REGS];
    int         IP;
    int         SP;
    int         BP;
    Instruction IR;
    int         instrCount;
    Memory&     mem;

    std::map<std::string, double> vars;
    std::vector<double>           freg;
    std::vector<double>           stack;

    explicit Processor(Memory& m, int count);

    bool step();
    void run();
    void printState() const;

private:
    void fetch();
    void decode();
    void execute();
    bool jumped;
};
