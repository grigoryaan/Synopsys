#include <iostream>

static int a = 10; 

void foo() {
    std::cout << "a = : " << a << std::endl;
}

int main() {
    foo();
    a = 20; 
    foo();
    return 0;
}
