#pragma once
#include "ast.h"
#include <vector>
#include <map>
#include <string>

enum class OpCode {
    LOAD_CONST, LOAD_VAR, STORE_VAR,
    ADD, SUB, MUL, DIV,
    CMP_GT, CMP_LT, CMP_EQ, CMP_NEQ, CMP_GTE, CMP_LTE,
    JMP,
    BR_GT, BR_LT, BR_EQ, BR_NEQ, BR_GTE, BR_LTE,
    JA,
    NOP
};

struct Instruction {
    OpCode      op      = OpCode::NOP;
    int         rs1     = -1;
    int         rs2     = -1;
    int         rd      = -1;
    int         offset  =  0;
    double      value   =  0.0;
    std::string varName;
};

int         generate(NodePtr node, std::vector<Instruction>& code);
double      execute (std::vector<Instruction>& code, std::map<std::string,double>& vars);
const char* opName  (OpCode op);
