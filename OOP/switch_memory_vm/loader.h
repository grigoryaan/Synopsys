#pragma once
#include "memory.h"
#include "ir.h"
#include <vector>
#include <map>
#include <string>

int  load(const std::vector<Instruction>& code, Memory& mem);

double executeFromMemory(Memory& mem, int instrCount,
                         std::map<std::string,double>& vars);
