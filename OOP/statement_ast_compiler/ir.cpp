#include "ir.h"
#include "ast.h"

int generate(shared_ptr<Node> node, vector<Instruction>& code) {

    if (node->type == NodeType::IF) {

        int cond = generate(node->condition, code);

        Instruction jmpFalse;
        jmpFalse.op = OpCode::JMP_IF_FALSE;
        jmpFalse.left = cond;
        jmpFalse.dest = -1;

        int jmpIndex = code.size();
        code.push_back(jmpFalse);

        generate(node->body, code);

        if (node->elseBranch) {

            Instruction jmpEnd{OpCode::JMP};
            int jmpEndIndex = code.size();
            code.push_back(jmpEnd);

            code[jmpIndex].dest = code.size();

            generate(node->elseBranch, code);

            code[jmpEndIndex].dest = code.size();
        } else {
            code[jmpIndex].dest = code.size();
        }

        return -1;
    }

    if (node->type == NodeType::WHILE) {

        int start = code.size();

        int cond = generate(node->condition, code);

        Instruction jmpFalse{OpCode::JMP_IF_FALSE};
        jmpFalse.left = cond;

        int jmpIndex = code.size();
        code.push_back(jmpFalse);

        generate(node->body, code);

        code.push_back({OpCode::JMP, start});

        code[jmpIndex].dest = code.size();

        return -1;
    }

    return -1;
}
