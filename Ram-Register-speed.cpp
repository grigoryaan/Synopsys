#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

volatile int y = 1;

int main() {
    const int N = 200000000; 
    long long sum = 0;       

    int x = 1; 
    auto t1 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) sum += x; 
    auto t2 = high_resolution_clock::now();


    sum = 0;
    auto t3 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) sum += y; 
    auto t4 = high_resolution_clock::now();

    std::vector<int> arr(N, 1); 
    sum = 0;
    auto t5 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) sum += arr[i]; 
    auto t6 = high_resolution_clock::now();

    std::cout << "Regster time "<< duration_cast<milliseconds>(t2 - t1).count() << " ms" << std::endl;

    std::cout << "Volatile Time "<< duration_cast<milliseconds>(t4 - t3).count() << " ms" << std::endl;

    std::cout << "RAM Time "<< duration_cast<milliseconds>(t6 - t5).count() << " ms" << std::endl;
}
