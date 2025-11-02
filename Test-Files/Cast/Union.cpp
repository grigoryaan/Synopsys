#include <iostream>

union Data {
    int i; float f; char c;
};

int main() {
    Data data;

    data.i = 65; 
    std::cout << "i = " << data.i << std::endl;

    data.c = 'A'; 
    std::cout << "c = " << data.c << std::endl;

    data.f = 3.14;
    std::cout << "f = " << data.f << std::endl;

    return 0;
}
