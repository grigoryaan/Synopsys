#pragma once
#include "ir.h"
#include <vector>
#include <string>
#include <map>
#include <cstdint>

struct SymbolEntry {
    std::string name;
    int         offset;
    std::string type;
};

struct RelocEntry {
    int instrIndex;
    std::string symbolName;
};

struct JumpTableEntry {
    std::string name;
    int         offset;
};

struct ExecFile {
    std::vector<Instruction>   code;
    std::vector<SymbolEntry>   symbolTable;
    std::vector<RelocEntry>    relocTable;
    std::vector<JumpTableEntry>jumpTable;
    int                        baseAddress = 0;
};

ExecFile buildExecFile(const std::vector<Instruction>& code,
                       const std::map<std::string,int>& funcAddrs);

void printExecFile(const ExecFile& ef);
