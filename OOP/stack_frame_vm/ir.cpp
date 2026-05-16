#include "ir.h"
#include <stdexcept>
#include <map>

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
        case OpCode::PUSH:       return "PUSH";
        case OpCode::POP:        return "POP";
        case OpCode::CALL:       return "CALL";
        case OpCode::RET:        return "RET";
        case OpCode::LOAD_BP:    return "LOAD_BP";
        case OpCode::STORE_BP:   return "STORE_BP";
        case OpCode::MOV_BP_SP:  return "MOV_BP_SP";
        case OpCode::MOV_SP_BP:  return "MOV_SP_BP";
        case OpCode::SUB_SP:     return "SUB_SP";
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
        Instruction br; br.op = negatedBranch(cond->value);
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

static std::map<std::string, int>                    g_funcAddr;
static std::vector<std::pair<int,std::string>>       g_callPatches;

// Each function call gets a unique call-id to scope its local vars
static int g_callId = 0;

void patchCalls(std::vector<Instruction>& code) {
    for (auto& [idx, name] : g_callPatches) {
        if (!g_funcAddr.count(name))
            throw std::runtime_error("Undefined function: " + name);
        code[idx].offset = g_funcAddr[name];
    }
    g_callPatches.clear();
}

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

    // FUNC_DEF:
    // prologue: PUSH BP / MOV_BP_SP / SUB_SP n
    // params: PUSH-ված args-ն stack-ից կարդում ենք LOAD_BP-ով
    // body
    // epilogue: MOV_SP_BP / POP BP / RET
    case NodeKind::FUNC_DEF: {
        Instruction skipJmp; skipJmp.op = OpCode::JMP; skipJmp.rd = (int)code.size();
        code.push_back(skipJmp);

        g_funcAddr[node->value] = (int)code.size();

        { Instruction ins; ins.op = OpCode::PUSH; ins.varName = "BP"; ins.rd = (int)code.size(); code.push_back(ins); }
        { Instruction ins; ins.op = OpCode::MOV_BP_SP; ins.rd = (int)code.size(); code.push_back(ins); }

        int nParams = (int)node->params.size();
        { Instruction ins; ins.op = OpCode::SUB_SP; ins.offset = nParams * 4; ins.rd = (int)code.size(); code.push_back(ins); }

        // params are on stack BELOW the frame (pushed before CALL+PUSH_BP)
        // stack layout at MOV_BP_SP:
        //   [arg0, arg1, ..., argN-1, retIP, BP_saved]  <- SP (BP = SP here)
        // BP-1 = BP_saved (just pushed), BP-2 = retIP, BP-3 = last arg, ...
        // arg[i] is at BP - (nParams + 2) + i
        for (int i = 0; i < nParams; i++) {
            Instruction ins; ins.op = OpCode::LOAD_BP;
            // offset encodes stack-relative index (in units of 4)
            ins.offset  = (-i - 3) * 4;
            ins.varName = node->params[i];
            ins.rd      = (int)code.size();
            code.push_back(ins);
            Instruction store; store.op = OpCode::STORE_VAR;
            store.rs1 = ins.rd; store.varName = node->params[i];
            store.rd  = (int)code.size(); code.push_back(store);
        }

        generate(node->body, code);

        { Instruction ins; ins.op = OpCode::MOV_SP_BP; ins.rd = (int)code.size(); code.push_back(ins); }
        { Instruction ins; ins.op = OpCode::POP; ins.varName = "BP"; ins.rd = (int)code.size(); code.push_back(ins); }
        { Instruction ins; ins.op = OpCode::RET; ins.rd = (int)code.size(); code.push_back(ins); }

        code[skipJmp.rd].offset = relOffset(skipJmp.rd, (int)code.size());
        return newReg(code);
    }

    case NodeKind::RETURN: {
        int val = generate(node->left, code);
        { Instruction ins; ins.op = OpCode::STORE_VAR; ins.rs1 = val;
          ins.varName = "__retval"; ins.rd = (int)code.size(); code.push_back(ins); }
        { Instruction ins; ins.op = OpCode::MOV_SP_BP; ins.rd = (int)code.size(); code.push_back(ins); }
        { Instruction ins; ins.op = OpCode::POP; ins.varName = "BP"; ins.rd = (int)code.size(); code.push_back(ins); }
        { Instruction ins; ins.op = OpCode::RET; ins.rd = (int)code.size(); code.push_back(ins); }
        return newReg(code);
    }

    // FUNC_CALL: push args right-to-left, CALL, pop args, load __retval
    case NodeKind::FUNC_CALL: {
        for (int i = (int)node->stmts.size()-1; i >= 0; i--) {
            int argRd = generate(node->stmts[i], code);
            Instruction push; push.op = OpCode::PUSH; push.rs1 = argRd;
            push.rd = (int)code.size(); code.push_back(push);
        }
        Instruction call; call.op = OpCode::CALL;
        call.varName = node->value; call.rd = (int)code.size();
        g_callPatches.push_back({call.rd, node->value});
        code.push_back(call);

        // pop args
        { Instruction ins; ins.op = OpCode::SUB_SP;
          ins.offset = -(int)node->stmts.size() * 4;
          ins.rd = (int)code.size(); code.push_back(ins); }

        Instruction ret; ret.op = OpCode::LOAD_VAR;
        ret.varName = "__retval"; ret.rd = (int)code.size();
        code.push_back(ret);
        return ret.rd;
    }

    default:
        throw std::runtime_error("generate: unhandled NodeKind");
    }
}

double execute(std::vector<Instruction>& code, std::map<std::string,double>& vars) {
    const int N = (int)code.size();
    std::vector<double> reg(N, 0.0);
    std::vector<double> stack;
    int SP = 0, BP = 0, pc = 0;

    auto stackGet = [&](int idx) -> double {
        return (idx >= 0 && idx < (int)stack.size()) ? stack[idx] : 0.0;
    };

    while (pc >= 0 && pc < N) {
        auto& ins = code[pc];
        switch (ins.op) {
        case OpCode::LOAD_CONST: reg[ins.rd] = ins.value; break;
        case OpCode::LOAD_VAR:   reg[ins.rd] = vars.count(ins.varName) ? vars[ins.varName] : 0.0; break;
        case OpCode::STORE_VAR:  vars[ins.varName] = reg[ins.rs1]; reg[ins.rd] = reg[ins.rs1]; break;
        case OpCode::ADD:  reg[ins.rd] = reg[ins.rs1] + reg[ins.rs2]; break;
        case OpCode::SUB:  reg[ins.rd] = reg[ins.rs1] - reg[ins.rs2]; break;
        case OpCode::MUL:  reg[ins.rd] = reg[ins.rs1] * reg[ins.rs2]; break;
        case OpCode::DIV:  reg[ins.rd] = reg[ins.rs1] / reg[ins.rs2]; break;
        case OpCode::CMP_GT:  reg[ins.rd] = reg[ins.rs1] >  reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_LT:  reg[ins.rd] = reg[ins.rs1] <  reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_EQ:  reg[ins.rd] = reg[ins.rs1] == reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_NEQ: reg[ins.rd] = reg[ins.rs1] != reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_GTE: reg[ins.rd] = reg[ins.rs1] >= reg[ins.rs2] ? 1:0; break;
        case OpCode::CMP_LTE: reg[ins.rd] = reg[ins.rs1] <= reg[ins.rs2] ? 1:0; break;
        case OpCode::JMP: pc = pc + 1 + ins.offset; continue;
        case OpCode::JA: {
            uint32_t a=(uint32_t)reg[ins.rs1], b=(uint32_t)reg[ins.rs2];
            if (a > b) { pc=pc+1+ins.offset; continue; } break;
        }
        case OpCode::BR_GT:  if (reg[ins.rs1] >  reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_LT:  if (reg[ins.rs1] <  reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_EQ:  if (reg[ins.rs1] == reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_NEQ: if (reg[ins.rs1] != reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_GTE: if (reg[ins.rs1] >= reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::BR_LTE: if (reg[ins.rs1] <= reg[ins.rs2]) { pc=pc+1+ins.offset; continue; } break;
        case OpCode::PUSH:
            stack.push_back(ins.varName == "BP" ? (double)BP : reg[ins.rs1]);
            SP++; break;
        case OpCode::POP:
            if (!stack.empty()) {
                double v = stack.back(); stack.pop_back(); SP--;
                if (ins.varName == "BP") BP = (int)v;
            } break;
        case OpCode::MOV_BP_SP: BP = SP; break;
        case OpCode::MOV_SP_BP: SP = BP; stack.resize(SP, 0.0); break;
        case OpCode::SUB_SP:
            SP += ins.offset / 4;
            if (SP < 0) SP = 0;
            stack.resize(SP, 0.0);
            break;
        case OpCode::LOAD_BP: {
            int idx = BP + ins.offset / 4;
            reg[ins.rd] = stackGet(idx);
            if (!ins.varName.empty()) vars[ins.varName] = reg[ins.rd];
            break;
        }
        case OpCode::STORE_BP: {
            int idx = BP + ins.offset / 4;
            if (idx >= 0 && idx < (int)stack.size()) stack[idx] = reg[ins.rs1];
            break;
        }
        case OpCode::CALL:
            stack.push_back((double)(pc + 1)); SP++;
            pc = ins.offset; continue;
        case OpCode::RET:
            if (!stack.empty()) {
                int retAddr = (int)stack.back(); stack.pop_back(); SP--;
                pc = retAddr; continue;
            }
            pc = N; continue;
        case OpCode::NOP: break;
        }
        ++pc;
    }
    return reg.empty() ? 0.0 : reg.back();
}
