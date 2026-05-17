#pragma once
#include "exec_file.h"
#include "memory.h"

void relocate(ExecFile& ef, int baseAddress);

int  loadExecFile(ExecFile& ef, Memory& mem);
