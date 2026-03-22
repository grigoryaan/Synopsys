#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cctype>

using namespace std;

enum TokenType {NUMBER, VARIABLE, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END};

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokens;
int pos = 0;

vector<string> asmCode;
int regCounter = 0;

string newReg() {
    return "r" + to_string(regCounter++);
}

struct Node {
    string value;
    Node* left;
    Node* right;

    Node(string v, Node* l=nullptr, Node* r=nullptr)
        : value(v), left(l), right(r) {}
};

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

Node* expression();

Node* factor() {

    Token t = tokens[pos++];

    if (t.type==NUMBER || t.type==VARIABLE)
        return new Node(t.value);

    if (t.type==LPAREN) {
        Node* node = expression();
        pos++;
        return node;
    }

    return nullptr;
}

Node* term() {

    Node* node = factor();

    while (tokens[pos].type==MUL || tokens[pos].type==DIV) {

        string op = tokens[pos++].value;
        Node* right = factor();

        node = new Node(op,node,right);
    }

    return node;
}

Node* expression() {

    Node* node = term();

    while (tokens[pos].type==PLUS || tokens[pos].type==MINUS) {

        string op = tokens[pos++].value;
        Node* right = term();

        node = new Node(op,node,right);
    }

    return node;
}

string generateASM(Node* node) {

    if (!node->left && !node->right) {
        string r = newReg();
        asmCode.push_back("MOV " + r + ", " + node->value);
        return r;
    }

    string left = generateASM(node->left);
    string right = generateASM(node->right);

    string r = newReg();

    if (node->value == "+")
        asmCode.push_back("ADD " + r + ", " + left + ", " + right);

    else if (node->value == "-")
        asmCode.push_back("SUB " + r + ", " + left + ", " + right);

    else if (node->value == "*")
        asmCode.push_back("MUL " + r + ", " + left + ", " + right);

    else if (node->value == "/")
        asmCode.push_back("DIV " + r + ", " + left + ", " + right);

    return r;
}

set<string> findVars(const string& s) {

    set<string> vars;
    string name;

    for(char c : s) {
        if(isalpha(c))
            name+=c;
        else {
            if(!name.empty()) {
                vars.insert(name);
                name.clear();
            }
        }
    }

    if(!name.empty())
        vars.insert(name);

    return vars;
}

double eval(Node* node, map<string,double>& vars) {

    if (!node->left && !node->right) {

        if (isalpha(node->value[0]))
            return vars[node->value];

        return stod(node->value);
    }

    double l = eval(node->left,vars);
    double r = eval(node->right,vars);

    if (node->value=="+") return l+r;
    if (node->value=="-") return l-r;
    if (node->value=="*") return l*r;
    if (node->value=="/") return l/r;

    return 0;
}

int main() {

    string input;

    cout<<"Enter expression: ";
    getline(cin,input);

    tokenize(input);

    pos = 0;
    Node* root = expression();

    string resultReg = generateASM(root);

    asmCode.push_back("HALT");

    cout<<"\nAssembly code:\n";

    for(auto &line : asmCode)
        cout<<line<<endl;

    set<string> vars = findVars(input);
    map<string,double> values;

    cout<<"\nResults:\n";

    for(double v=1; v<=100; v+=0.1) {

        for(auto &name:vars)
            values[name]=v;

        double result = eval(root,values);

        cout<<"value="<<v<<" result="<<result<<endl;
    }

    return 0;
}
