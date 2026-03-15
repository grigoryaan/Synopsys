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

vector<string> code;   // ← նկարում նշված vector
int tempCounter = 1;

string newTemp() {
    return "tmp" + to_string(tempCounter++);
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
            while (i<input.size() && isdigit(input[i]))
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

        Token op = tokens[pos++];

        Node* right = factor();

        node = new Node(op.value,node,right);
    }

    return node;
}

Node* expression() {

    Node* node = term();

    while (tokens[pos].type==PLUS || tokens[pos].type==MINUS) {

        Token op = tokens[pos++];

        Node* right = term();

        node = new Node(op.value,node,right);
    }

    return node;
}


string generate(Node* node) {

    if (!node->left && !node->right)
        return node->value;

    string left = generate(node->left);
    string right = generate(node->right);

    string tmp = newTemp();

    code.push_back(tmp + " = " + left + " " + node->value + " " + right);

    return tmp;
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

    Node* root = expression();

    string resultTemp = generate(root);

    code.push_back("return " + resultTemp);

    cout<<"\nVector instructions:\n";

    for(auto& c:code)
        cout<<c<<endl;


    set<string> vars = findVars(input);

    map<string,double> values;

    cout<<"\nResults:\n";

    for(double v=1; v<=100; v+=0.1) {

        for(auto& name:vars)
            values[name]=v;

        double result = eval(root,values);

        cout<<"value="<<v<<" result="<<result<<endl;
    }
}
