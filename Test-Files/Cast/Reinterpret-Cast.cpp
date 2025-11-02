#include <iostream>
int main() {
    int x = 48;
    char* p = reinterpret_cast<char*>(&x);
    std::cout << *p; // tpuma ASCII i axyusakin hamapatasxan 
}
