#include <iostream>
int main() {
    const int x = 10;
    int* ptr = const_cast<int*>(&x);
    *ptr = 20; 
    std::cout << *ptr;
}
