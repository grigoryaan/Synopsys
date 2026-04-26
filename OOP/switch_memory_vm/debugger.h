#pragma once
#include "memory.h"
#include "ir.h"
#include <set>
#include <map>
#include <string>
#include <vector>


struct Debugger {
    Memory&                       mem;
    int                           instrCount;
    std::map<std::string,double>& vars;

    std::set<int>         breakpoints;
    std::vector<double>   reg;   
    int                   pc = 0;
    bool                  done = false;

    Debugger(Memory& m, int count, std::map<std::string,double>& v);

    void addBreakpoint(int instrIdx);
    void removeBreakpoint(int instrIdx);

    bool step();

    void cont();

    void printState() const;

    void runREPL();

private:
    bool executeOne();
};
