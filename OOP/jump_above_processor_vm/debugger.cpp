#include "debugger.h"
#include <iostream>

Debugger::Debugger(Processor& p) : cpu(p) {}

void Debugger::addBreakpoint(int ip)    { breakpoints.insert(ip); }
void Debugger::removeBreakpoint(int ip) { breakpoints.erase(ip);  }

bool Debugger::step() {
    cpu.printState();
    return cpu.step();
}

void Debugger::cont() {
    while (cpu.IP >= 0 && cpu.IP < cpu.instrCount) {
        if (breakpoints.count(cpu.IP)) {
            std::cout << "[Debugger] Breakpoint @ IP=" << cpu.IP << "\n";
            cpu.printState();
            return;
        }
        cpu.step();
    }
    std::cout << "[Debugger] Program finished\n";
}

void Debugger::runREPL() {
    std::cout << "Debugger REPL — s:step  c:continue  b <n>:breakpoint  r <n>:remove  q:quit\n";
    std::string line;
    while (cpu.IP < cpu.instrCount && std::getline(std::cin, line)) {
        if (line.empty() || line == "s") {
            if (!step()) { std::cout << "[Debugger] Done\n"; break; }
        } else if (line == "c") {
            cont(); break;
        } else if (line.rfind("b ", 0) == 0) {
            int n = std::stoi(line.substr(2));
            addBreakpoint(n);
            std::cout << "BP set @ " << n << "\n";
        } else if (line.rfind("r ", 0) == 0) {
            int n = std::stoi(line.substr(2));
            removeBreakpoint(n);
            std::cout << "BP removed @ " << n << "\n";
        } else if (line == "q") {
            break;
        }
    }
}
