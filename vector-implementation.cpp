#include <iostream>

class MyVector {
private:
    int* data;       
    int size;        
    int capacity;    

    void resize() {
        capacity *= 2;
        int* newData = new int[capacity];
        for (int i = 0; i < size; i++)
            newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    MyVector() {
        capacity = 10;
        size = 0;
        data = new int[capacity];
    }

    ~MyVector() {
        delete[] data;
    }

    void push_back(int value) {
        if (size == capacity)
            resize();
        data[size++] = value;
    }

    void pop_back() {
        if (size > 0)
            size--;
    }

    int get_size() const {
        return size;
    }

    int get_capacity() const {
        return capacity;
    }

    
    int get(int index) const {
        if (index >= 0 && index < size)
            return data[index];
        return -1;
    }

    int set(int index, int value) {
        if (index >= 0 && index < size) {
            data[index] = value;
            return 0;
        }
        return -1;
    }

    
    int insert(int index, int value) {
        if (index < 0 || index > size)
            return -1;

        if (size == capacity)
            resize();

        for (int i = size; i > index; i--)
            data[i] = data[i - 1];

        data[index] = value;
        size++;
        return 0;
    }

    int remove(int index) {
        if (index < 0 || index >= size)
            return -1;

        for (int i = index; i < size - 1; i++)
            data[i] = data[i + 1];

        size--;
        return 0;
    }

    void clear() {
        size = 0;
    }

    int& operator[](int index) {
        return data[index];
    }

    void print() const {
        for (int i = 0; i < size; i++)
            std::cout << data[i] << " ";
        std::cout << std::endl;
    }
};


int main() {
    MyVector vec;

    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    int result = vec.insert(1, 15);
    if (result == -1)
        std::cout << "Insert failed: invalid index" << std::endl;
    else
        std::cout << "Insert succeeded at index 1" << std::endl;

    result = vec.insert(100, 99);
    if (result == -1)
        std::cout << "Insert failed: invalid index" << std::endl;
    else
        std::cout << "Insert succeeded at index 100" << std::endl;

    result = vec.remove(2);
    if (result == -1)
        std::cout << "Remove failed: invalid index" << std::endl;
    else
        std::cout << "Remove succeeded at index 2" << std::endl;

    result = vec.remove(-1);
    if (result == -1)
        std::cout << "Remove failed: invalid index" << std::endl;
    else
        std::cout << "Remove succeeded at index -1" << std::endl;

    result = vec.set(0, 5);
    if (result == -1)
        std::cout << "Set failed: invalid index" << std::endl;
    else
        std::cout << "Set succeeded at index 0" << std::endl;

    result = vec.set(10, 99);
    if (result == -1)
        std::cout << "Set failed: invalid index" << std::endl;
    else
        std::cout << "Set succeeded at index 10" << std::endl;

    int value = vec.get(1);
    if (value == -1)
        std::cout << "Get failed: invalid index" << std::endl;
    else
        std::cout << "Value at index 1: " << value << std::endl;

    value = vec.get(100);
    if (value == -1)
        std::cout << "Get failed: invalid index" << std::endl;
    else
        std::cout << "Value at index 100: " << value << std::endl;

    std::cout << "Final vector elements: ";
    vec.print();

    return 0;
}
