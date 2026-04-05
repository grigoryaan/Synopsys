#pragma once
#include <vector>

enum class OpCode {
    LOAD_CONST,
    LOAD_VAR,
    ADD, SUB, MUL, DIV,
    CMP,
    JMP,
    JMP_IF_FALSE
};

struct Instruction {
    OpCode op;
    int left;
    int right;
    int dest;
    double value;
};
