#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <cctype>

using namespace std;

// ---------- TOKENS ----------
enum TokenType {NUMBER, VARIABLE, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END};

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokens;
int pos = 0;

// ---------- AST ----------
struct Node {
    string value;
    shared_ptr<Node> left;
    shared_ptr<Node> right;

    Node(string v) : value(v), left(nullptr), right(nullptr) {}
};

// ---------- TOKENIZER ----------
void tokenize(const string& input) {

    tokens.clear();

    for (int i=0;i<input.size();i++) {

        if (isspace(input[i])) continue;

        if (isdigit(input[i])) {
            string num;
            while (i<input.size() && (isdigit(input[i]) || input[i]=='.'))
                num+=input[i++];
            i--;
            tokens.push_back({NUMBER,num});
        }

        else if (isalpha(input[i])) {
            string var;
            while (i<input.size() && isalpha(input[i]))
                var+=input[i++];
            i--;
            tokens.push_back({VARIABLE,var});
        }

        else if (input[i]=='+') tokens.push_back({PLUS,"+"});
        else if (input[i]=='-') tokens.push_back({MINUS,"-"});
        else if (input[i]=='*') tokens.push_back({MUL,"*"});
        else if (input[i]=='/') tokens.push_back({DIV,"/"});
        else if (input[i]=='(') tokens.push_back({LPAREN,"("});
        else if (input[i]==')') tokens.push_back({RPAREN,")"});
    }

    tokens.push_back({END,""});
}

// ---------- PARSER ----------
shared_ptr<Node> expression();

shared_ptr<Node> factor() {

    Token t = tokens[pos++];

    if (t.type==NUMBER || t.type==VARIABLE)
        return make_shared<Node>(t.value);

    if (t.type==LPAREN) {
        auto node = expression();
        pos++;
        return node;
    }

    return nullptr;
}

shared_ptr<Node> term() {

    auto node = factor();

    while (tokens[pos].type==MUL || tokens[pos].type==DIV) {

        string op = tokens[pos++].value;
        auto right = factor();

        auto newNode = make_shared<Node>(op);
        newNode->left = node;
        newNode->right = right;

        node = newNode;
    }

    return node;
}

shared_ptr<Node> expression() {

    auto node = term();

    while (tokens[pos].type==PLUS || tokens[pos].type==MINUS) {

        string op = tokens[pos++].value;
        auto right = term();

        auto newNode = make_shared<Node>(op);
        newNode->left = node;
        newNode->right = right;

        node = newNode;
    }

    return node;
}

// ---------- IR (Instruction) ----------
enum class OpCode {
    LOAD_CONST,
    LOAD_VAR,
    ADD,
    SUB,
    MUL,
    DIV
};

struct Instruction {
    OpCode op;
    int left;
    int right;
    int dest;
    double value;
};

// ---------- GENERATE IR ----------
int generate(shared_ptr<Node> node, vector<Instruction>& code) {

    // leaf
    if (!node->left && !node->right) {

        Instruction instr;
        instr.dest = code.size();
        instr.left = -1;
        instr.right = -1;

        if (isalpha(node->value[0])) {
            instr.op = OpCode::LOAD_VAR;
        } else {
            instr.op = OpCode::LOAD_CONST;
            instr.value = stod(node->value);
        }

        code.push_back(instr);
        return instr.dest;
    }

    int l = generate(node->left, code);
    int r = generate(node->right, code);

    Instruction instr;
    instr.left = l;
    instr.right = r;
    instr.dest = code.size();

    if (node->value == "+") instr.op = OpCode::ADD;
    if (node->value == "-") instr.op = OpCode::SUB;
    if (node->value == "*") instr.op = OpCode::MUL;
    if (node->value == "/") instr.op = OpCode::DIV;

    code.push_back(instr);

    return instr.dest;
}

// ---------- EXECUTE ----------
double execute(vector<Instruction>& code, map<string,double>& vars) {

    vector<double> memory(code.size());

    for (int i = 0; i < code.size(); i++) {

        auto& ins = code[i];

        switch(ins.op) {

            case OpCode::LOAD_CONST:
                memory[i] = ins.value;
                break;

            case OpCode::LOAD_VAR:
                memory[i] = vars["x"]; 
                break;

            case OpCode::ADD:
                memory[i] = memory[ins.left] + memory[ins.right];
                break;

            case OpCode::SUB:
                memory[i] = memory[ins.left] - memory[ins.right];
                break;

            case OpCode::MUL:
                memory[i] = memory[ins.left] * memory[ins.right];
                break;

            case OpCode::DIV:
                memory[i] = memory[ins.left] / memory[ins.right];
                break;
        }
    }

    return memory.back();
}

// ---------- MAIN ----------
int main() {

    string input;
    cout << "Enter expression: ";
    getline(cin, input);

    tokenize(input);
    pos = 0;

    auto root = expression();

    vector<Instruction> code;
    generate(root, code);

    cout << "\nInstruction Vector:\n";

    for (int i = 0; i < code.size(); i++) {
        cout << i << " ";
        cout << (int)code[i].op << " ";
        cout << code[i].left << " ";
        cout << code[i].right << " ";
        cout << code[i].dest << endl;
    }

    map<string,double> vars;

    cout << "\nResults:\n";

    for(double x = 1; x <= 100; x += 0.1) {
        vars["x"] = x;

        double res = execute(code, vars);

        cout << "x=" << x << " result=" << res << endl;
    }
}
