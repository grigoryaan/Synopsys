#pragma once
#include "ast.h"
#include <vector>
#include <map>
#include <string>

enum class OpCode {
    LOAD_CONST,
    LOAD_VAR,
    STORE_VAR,
    ADD, SUB, MUL, DIV,
    CMP_LT, CMP_GT, CMP_LE, CMP_GE, CMP_EQ, CMP_NEQ,
    JMP,       
    JZ,        
    NOP       
};

struct Instruction {
    OpCode op;
    int    left  = -1;
    int    right = -1;
    int    dest  = -1;   
    double value = 0.0;
    std::string varName;
};


int generate(NodePtr node, std::vector<Instruction>& code);

double execute(std::vector<Instruction>& code,
               std::map<std::string, double>& vars);
