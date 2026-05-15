#include "ir.h"
#include <stdexcept>

const char* opName(OpCode op) {
    switch (op) {
        case OpCode::LOAD_CONST: return "LOAD_CONST";
        case OpCode::LOAD_VAR:   return "LOAD_VAR";
        case OpCode::STORE_VAR:  return "STORE_VAR";
        case OpCode::ADD:        return "ADD";
        case OpCode::SUB:        return "SUB";
        case OpCode::MUL:        return "MUL";
        case OpCode::DIV:        return "DIV";
        case OpCode::CMP_GT:     return "CMP_GT";
        case OpCode::CMP_LT:     return "CMP_LT";
        case OpCode::CMP_EQ:     return "CMP_EQ";
        case OpCode::CMP_NEQ:    return "CMP_NEQ";
        case OpCode::CMP_GTE:    return "CMP_GTE";
        case OpCode::CMP_LTE:    return "CMP_LTE";
        case OpCode::JMP:        return "JMP";
        case OpCode::BR_GT:      return "BR_GT";
        case OpCode::BR_LT:      return "BR_LT";
        case OpCode::BR_EQ:      return "BR_EQ";
        case OpCode::BR_NEQ:     return "BR_NEQ";
        case OpCode::BR_GTE:     return "BR_GTE";
        case OpCode::BR_LTE:     return "BR_LTE";
        case OpCode::JA:         return "JA";
        case OpCode::NOP:        return "NOP";
        default:                 return "???";
    }
}

static int newReg(std::vector<Instruction>& code) {
    Instruction ins; ins.op = OpCode::NOP; ins.rd = (int)code.size();
    code.push_back(ins); return ins.rd;
}

static int relOffset(int from, int to) { return to - from - 1; }

static bool isCompOp(const std::string& op) {
    return op==">"||op=="<"||op==">="||op=="<="||op=="=="||op=="!=";
}

static OpCode negatedBranch(const std::string& op) {
    if (op == ">")  return OpCode::BR_LTE;
    if (op == "<")  return OpCode::BR_GTE;
    if (op == ">=") return OpCode::BR_LT;
    if (op == "<=") return OpCode::BR_GT;
    if (op == "==") return OpCode::BR_NEQ;
    if (op == "!=") return OpCode::BR_EQ;
    throw std::runtime_error("negatedBranch: " + op);
}

static OpCode cmpOpcode(const std::string& op) {
    if (op == ">")  return OpCode::CMP_GT;
    if (op == "<")  return OpCode::CMP_LT;
    if (op == ">=") return OpCode::CMP_GTE;
    if (op == "<=") return OpCode::CMP_LTE;
    if (op == "==") return OpCode::CMP_EQ;
    if (op == "!=") return OpCode::CMP_NEQ;
    throw std::runtime_error("cmpOpcode: " + op);
}

static int generateBranch(NodePtr cond, std::vector<Instruction>& code) {
    if (cond->kind == NodeKind::BINOP && isCompOp(cond->value)) {
        int rs1 = generate(cond->left,  code);
        int rs2 = generate(cond->right, code);
        Instruction br;
        br.op = negatedBranch(cond->value);
        br.rs1 = rs1; br.rs2 = rs2; br.rd = (int)code.size();
        code.push_back(br); return br.rd;
    }
    int val = generate(cond, code);
    Instruction lc; lc.op = OpCode::LOAD_CONST; lc.value = 0.0; lc.rd = (int)code.size();
    code.push_back(lc);
    Instruction br; br.op = OpCode::BR_EQ; br.rs1 = val; br.rs2 = lc.rd; br.rd = (int)code.size();
    code.push_back(br); return br.rd;
}

static std::vector<int>* g_breakPatches = nullptr;

int generate(NodePtr node, std::vector<Instruction>& code) {
    if (!node) return -1;

    switch (node->kind) {

    case NodeKind::NUMBER: {
        Instruction ins; ins.op = OpCode::LOAD_CONST;
        ins.value = std::stod(node->value); ins.rd = (int)code.size();
        code.push_back(ins); return ins.rd;
    }

    case NodeKind::VARIABLE: {
        Instruction ins; ins.op = OpCode::LOAD_VAR;
        ins.varName = node->value; ins.rd = (int)code.size();
        code.push_back(ins); return ins.rd;
    }

    case NodeKind::BINOP: {
        int l = generate(node->left,  code);
        int r = generate(node->right, code);
        Instruction ins; ins.rs1 = l; ins.rs2 = r; ins.rd = (int)code.size();
        const std::string& op = node->value;
        if      (op=="+") ins.op = OpCode::ADD;
        else if (op=="-") ins.op = OpCode::SUB;
        else if (op=="*") ins.op = OpCode::MUL;
        else if (op=="/") ins.op = OpCode::DIV;
        else if (isCompOp(op)) ins.op = cmpOpcode(op);
        else throw std::runtime_error("Unknown op: " + op);
        code.push_back(ins); return ins.rd;
    }

    case NodeKind::ASSIGN: {
        int src = generate(node->right, code);
        Instruction ins; ins.op = OpCode::STORE_VAR;
        ins.rs1 = src; ins.varName = node->value; ins.rd = (int)code.size();
        code.push_back(ins); return ins.rd;
    }

    case NodeKind::EXPR_STMT:
    case NodeKind::RETURN:
        return generate(node->left, code);

    case NodeKind::BREAK: {
        Instruction jmp; jmp.op = OpCode::JMP; jmp.rd = (int)code.size();
        code.push_back(jmp);
        if (g_breakPatches) g_breakPatches->push_back(jmp.rd);
        return jmp.rd;
    }

    case NodeKind::BLOCK: {
        int last = -1;
        for (auto& s : node->stmts) last = generate(s, code);
        return last;
    }

    case NodeKind::IF: {
        int brIdx = generateBranch(node->cond, code);
        generate(node->body, code);
        int jmpIdx = -1;
        if (node->alt) {
            Instruction jmp; jmp.op = OpCode::JMP; jmp.rd = (int)code.size();
            code.push_back(jmp); jmpIdx = jmp.rd;
        }
        code[brIdx].offset = relOffset(brIdx, (int)code.size());
        if (node->alt) {
            generate(node->alt, code);
            code[jmpIdx].offset = relOffset(jmpIdx, (int)code.size());
        }
        return newReg(code);
    }

    case NodeKind::WHILE: {
        std::vector<int> bp; auto* prev = g_breakPatches; g_breakPatches = &bp;
        int loopStart = (int)code.size();
        int brIdx = generateBranch(node->cond, code);
        generate(node->body, code);
        Instruction jmp; jmp.op = OpCode::JMP;
        jmp.offset = relOffset((int)code.size(), loopStart);
        jmp.rd = (int)code.size(); code.push_back(jmp);
        code[brIdx].offset = relOffset(brIdx, (int)code.size());
        for (int idx : bp) code[idx].offset = relOffset(idx, (int)code.size());
        g_breakPatches = prev;
        return newReg(code);
    }

    case NodeKind::DO_WHILE: {
        std::vector<int> bp; auto* prev = g_breakPatches; g_breakPatches = &bp;
        int loopStart = (int)code.size();
        generate(node->body, code);
        if (node->cond->kind == NodeKind::BINOP && isCompOp(node->cond->value)) {
            int rs1 = generate(node->cond->left,  code);
            int rs2 = generate(node->cond->right, code);
            static auto posBranch = [](const std::string& op) -> OpCode {
                if (op==">")  return OpCode::BR_GT;
                if (op=="<")  return OpCode::BR_LT;
                if (op==">=") return OpCode::BR_GTE;
                if (op=="<=") return OpCode::BR_LTE;
                if (op=="==") return OpCode::BR_EQ;
                return OpCode::BR_NEQ;
            };
            Instruction br; br.op = posBranch(node->cond->value);
            br.rs1 = rs1; br.rs2 = rs2;
            br.offset = relOffset((int)code.size(), loopStart);
            br.rd = (int)code.size(); code.push_back(br);
        }
        for (int idx : bp) code[idx].offset = relOffset(idx, (int)code.size());
        g_breakPatches = prev;
        return newReg(code);
    }

    case NodeKind::SWITCH: {
        std::vector<int> bp; auto* prev = g_breakPatches; g_breakPatches = &bp;
        int exprRd = generate(node->cond, code);
        for (auto& caseNode : node->stmts) {
            if (caseNode->kind == NodeKind::DEFAULT_CASE) {
                for (auto& s : caseNode->stmts) generate(s, code);
                continue;
            }
            int valRd = generate(caseNode->cond, code);
            Instruction br; br.op = OpCode::BR_NEQ;
            br.rs1 = exprRd; br.rs2 = valRd; br.rd = (int)code.size();
            code.push_back(br); int brIdx = br.rd;
            for (auto& s : caseNode->stmts) generate(s, code);
            Instruction jmp; jmp.op = OpCode::JMP; jmp.rd = (int)code.size();
            code.push_back(jmp); bp.push_back(jmp.rd);
            code[brIdx].offset = relOffset(brIdx, (int)code.size());
        }
        for (int idx : bp) code[idx].offset = relOffset(idx, (int)code.size());
        g_breakPatches = prev;
        return newReg(code);
    }

    default:
        throw std::runtime_error("generate: unhandled NodeKind");
    }
}

double execute(std::vector<Instruction>& code, std::map<std::string,double>& vars) {
    const int N = (int)code.size();
    std::vector<double> mem(N, 0.0);
    int pc = 0;
    while (pc < N) {
        auto& ins = code[pc];
        switch (ins.op) {
        case OpCode::LOAD_CONST: mem[ins.rd] = ins.value; break;
        case OpCode::LOAD_VAR:   mem[ins.rd] = vars.count(ins.varName) ? vars[ins.varName] : 0.0; break;
        case OpCode::STORE_VAR:  vars[ins.varName] = mem[ins.rs1]; mem[ins.rd] = mem[ins.rs1]; break;
        case OpCode::ADD:  mem[ins.rd] = mem[ins.rs1] + mem[ins.rs2]; break;
        case OpCode::SUB:  mem[ins.rd] = mem[ins.rs1] - mem[ins.rs2]; break;
        case OpCode::MUL:  mem[ins.rd] = mem[ins.rs1] * mem[ins.rs2]; break;
        case OpCode::DIV:  mem[ins.rd] = mem[ins.rs1] / mem[ins.rs2]; break;
        case OpCode::CMP_GT:  mem[ins.rd] = mem[ins.rs1] >  mem[ins.rs2] ? 1:0; break;
        case OpCode::CMP_LT:  mem[ins.rd] = mem[ins.rs1] <  mem[ins.rs2] ? 1:0; break;
        case OpCode::CMP_EQ:  mem[ins.rd] = mem[ins.rs1] == mem[ins.rs2] ? 1:0; break;
        case OpCode::CMP_NEQ: mem[ins.rd] = mem[ins.rs1] != mem[ins.rs2] ? 1:0; break;
        case OpCode::CMP_GTE: mem[ins.rd] = mem[ins.rs1] >= mem[ins.rs2] ? 1:0; break;
        case OpCode::CMP_LTE: mem[ins.rd] = mem[ins.rs1] <= mem[ins.rs2] ? 1:0; break;
        case OpCode::JMP: pc = pc + 1 + ins.offset; continue;
        case OpCode::JA: {
            uint32_t a = (uint32_t)mem[ins.rs1];
            uint32_t b = (uint32_t)mem[ins.rs2];
            if (a > b) { pc = pc + 1 + ins.offset; continue; }
            break;
        }
        case OpCode::BR_GT:  if (mem[ins.rs1] >  mem[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_LT:  if (mem[ins.rs1] <  mem[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_EQ:  if (mem[ins.rs1] == mem[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_NEQ: if (mem[ins.rs1] != mem[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_GTE: if (mem[ins.rs1] >= mem[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_LTE: if (mem[ins.rs1] <= mem[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::NOP: break;
        }
        ++pc;
    }
    return mem.empty() ? 0.0 : mem.back();
}
