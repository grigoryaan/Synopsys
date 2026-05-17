#pragma once
#include "processor.h"
#include <set>

// Step-by-step debugger built on top of Processor.
// Uses Processor's Fetch→Decode→Execute for each step.
struct Debugger {
    Processor&  cpu;
    std::set<int> breakpoints;

    explicit Debugger(Processor& p);

    void addBreakpoint(int ip);
    void removeBreakpoint(int ip);

    // One Fetch→Decode→Execute step with state print.
    bool step();

    // Run until breakpoint or end.
    void cont();

    // Interactive REPL: s=step  c=continue  b <n>=breakpoint  r <n>=remove  q=quit
    void runREPL();
};
