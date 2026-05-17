#pragma once
#include "memory.h"
#include "ir.h"
#include <vector>

int load(const std::vector<Instruction>& code, Memory& mem);
