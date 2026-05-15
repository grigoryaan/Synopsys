#pragma once
#include "memory.h"
#include "ir.h"
#include <vector>

// Loads compiled IR into Memory code segment.
// Returns number of instructions loaded.
int load(const std::vector<Instruction>& code, Memory& mem);
