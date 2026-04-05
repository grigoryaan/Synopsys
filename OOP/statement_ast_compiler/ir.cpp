#include "ir.h"
#include <stdexcept>

// ── helpers 

static int newReg(std::vector<Instruction>& code) {
    Instruction ins;
    ins.op   = OpCode::NOP;
    ins.dest = (int)code.size();
    code.push_back(ins);
    return ins.dest;
}

// ── Code generation 

int generate(NodePtr node, std::vector<Instruction>& code) {
    if (!node) return -1;

    switch (node->kind) {

    // ── Leaves 
    case NodeKind::NUMBER: {
        Instruction ins;
        ins.op    = OpCode::LOAD_CONST;
        ins.value = std::stod(node->value);
        ins.dest  = (int)code.size();
        code.push_back(ins);
        return ins.dest;
    }

    case NodeKind::VARIABLE: {
        Instruction ins;
        ins.op      = OpCode::LOAD_VAR;
        ins.varName = node->value;
        ins.dest    = (int)code.size();
        code.push_back(ins);
        return ins.dest;
    }

    // ── Binary expression 
    case NodeKind::BINOP: {
        int l = generate(node->left,  code);
        int r = generate(node->right, code);
        Instruction ins;
        ins.left  = l;
        ins.right = r;
        ins.dest  = (int)code.size();

        const std::string& op = node->value;
        if      (op == "+")  ins.op = OpCode::ADD;
        else if (op == "-")  ins.op = OpCode::SUB;
        else if (op == "*")  ins.op = OpCode::MUL;
        else if (op == "/")  ins.op = OpCode::DIV;
        else if (op == "<")  ins.op = OpCode::CMP_LT;
        else if (op == ">")  ins.op = OpCode::CMP_GT;
        else if (op == "<=") ins.op = OpCode::CMP_LE;
        else if (op == ">=") ins.op = OpCode::CMP_GE;
        else if (op == "==") ins.op = OpCode::CMP_EQ;
        else if (op == "!=") ins.op = OpCode::CMP_NEQ;
        else throw std::runtime_error("Unknown operator: " + op);

        code.push_back(ins);
        return ins.dest;
    }

    // ── Assignment 
    case NodeKind::ASSIGN: {
        int valReg = generate(node->right, code);
        Instruction ins;
        ins.op      = OpCode::STORE_VAR;
        ins.left    = valReg;
        ins.varName = node->value;
        ins.dest    = (int)code.size();
        code.push_back(ins);
        return ins.dest;
    }

    // ── Expression statement 
    case NodeKind::EXPR_STMT:
        return generate(node->left, code);

    // ── Return 
    case NodeKind::RETURN:
        return generate(node->left, code);

    // ── Block 
    case NodeKind::BLOCK: {
        int last = -1;
        for (auto& s : node->stmts)
            last = generate(s, code);
        return last;
    }


    case NodeKind::IF: {
        int condReg = generate(node->cond, code);

        int jzIdx = (int)code.size();
        Instruction jz;
        jz.op   = OpCode::JZ;
        jz.left = condReg;
        jz.dest = -1; 
        code.push_back(jz);

        generate(node->body, code);

        int jmpIdx = -1;
        if (node->alt) {

            jmpIdx = (int)code.size();
            Instruction jmp;
            jmp.op   = OpCode::JMP;
            jmp.dest = -1;
            code.push_back(jmp);
        }

        code[jzIdx].dest = (int)code.size();

        if (node->alt) {
            generate(node->alt, code);

            code[jmpIdx].dest = (int)code.size();
        }


        return newReg(code);
    }


    case NodeKind::WHILE: {
        int loopStart = (int)code.size();

        int condReg = generate(node->cond, code);

        int jzIdx = (int)code.size();
        Instruction jz;
        jz.op   = OpCode::JZ;
        jz.left = condReg;
        jz.dest = -1;
        code.push_back(jz);

        generate(node->body, code);

        Instruction jmp;
        jmp.op   = OpCode::JMP;
        jmp.dest = loopStart;
        code.push_back(jmp);

        code[jzIdx].dest = (int)code.size();

        return newReg(code);
    }

    default:
        throw std::runtime_error("generate: unknown NodeKind");
    }
}

// ── Execution 

double execute(std::vector<Instruction>& code,
               std::map<std::string, double>& vars) {

    std::vector<double> mem(code.size(), 0.0);
    int pc = 0;

    while (pc < (int)code.size()) {
        auto& ins = code[pc];

        switch (ins.op) {
        case OpCode::LOAD_CONST:
            mem[ins.dest] = ins.value;
            break;

        case OpCode::LOAD_VAR:
            mem[ins.dest] = vars.count(ins.varName) ? vars[ins.varName] : 0.0;
            break;

        case OpCode::STORE_VAR:
            vars[ins.varName] = mem[ins.left];
            mem[ins.dest]     = mem[ins.left];
            break;

        case OpCode::ADD: mem[ins.dest] = mem[ins.left] + mem[ins.right]; break;
        case OpCode::SUB: mem[ins.dest] = mem[ins.left] - mem[ins.right]; break;
        case OpCode::MUL: mem[ins.dest] = mem[ins.left] * mem[ins.right]; break;
        case OpCode::DIV: mem[ins.dest] = mem[ins.left] / mem[ins.right]; break;

        case OpCode::CMP_LT:  mem[ins.dest] = mem[ins.left] <  mem[ins.right] ? 1.0 : 0.0; break;
        case OpCode::CMP_GT:  mem[ins.dest] = mem[ins.left] >  mem[ins.right] ? 1.0 : 0.0; break;
        case OpCode::CMP_LE:  mem[ins.dest] = mem[ins.left] <= mem[ins.right] ? 1.0 : 0.0; break;
        case OpCode::CMP_GE:  mem[ins.dest] = mem[ins.left] >= mem[ins.right] ? 1.0 : 0.0; break;
        case OpCode::CMP_EQ:  mem[ins.dest] = mem[ins.left] == mem[ins.right] ? 1.0 : 0.0; break;
        case OpCode::CMP_NEQ: mem[ins.dest] = mem[ins.left] != mem[ins.right] ? 1.0 : 0.0; break;

        case OpCode::JMP:
            pc = ins.dest;
            continue; 

        case OpCode::JZ:
            if (mem[ins.left] == 0.0) { pc = ins.dest; continue; }
            break;

        case OpCode::NOP:
            break;
        }
        ++pc;
    }

    return mem.empty() ? 0.0 : mem.back();
}
