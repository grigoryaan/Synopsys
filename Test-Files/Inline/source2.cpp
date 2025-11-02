#include <iostream>
#include "header.hpp"

int main() {
    std::cout << "source 2: a = " << a << std::endl;
    foo(); 
    a += 5; 
    std::cout << "source2: a = " << a << std::endl;
    return 0;
}
