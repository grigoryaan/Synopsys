#include <iostream>

volatile int v = 0; 

void checkV() {
    while (v == 0) {

    }
    std::cout << "V-n poxvec: " << v << std::endl;
}

int main() {
    
    v = 1;

    checkV();
    return 0;
}
