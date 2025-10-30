#include <iostream>
#include <iomanip> 

int main() {
    double x = 1;
    while ((x + 1) != x) {
        x *= 2;
        std::cout << std::fixed << std::setprecision(0) << x << std::endl;

    }
    std::cout << "Minimal value: " << x << std::endl;




    double k = 9007199254740992; 
    double y = x + 1;            
    double z = nextafter(x, INFINITY); 

    std::cout << std::fixed << std::setprecision(0);
    std::cout << "x = " << x << std::endl;
    std::cout << "x + 1 = " << y << std::endl;
    std::cout << "x+1-x = " << y - x << std::endl;
    std::cout << "nextafter(x, +inf) = " << z << std::endl;
    std::cout << "difference = " << z - x << std::endl;
    return 0;
}
