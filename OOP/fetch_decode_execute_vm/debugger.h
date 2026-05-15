#pragma once
#include "processor.h"
#include <set>

struct Debugger {
    Processor&  cpu;
    std::set<int> breakpoints;

    explicit Debugger(Processor& p);

    void addBreakpoint(int ip);
    void removeBreakpoint(int ip);

    bool step();

    void cont();

    void runREPL();
};
