#include <iostream>

struct dyn_str {
    size_t m_size;       
    size_t m_capacity;   
    char* m_pointer;    
};

union u {
    dyn_str dynamic;       
    char    static_str[24]; 
};

class mystring {
private:
    u data;       
    size_t len;   

    bool is_small() const { 
        return len <= 23;}               

public:

    mystring(const char* text) {
        len = std::strlen(text);

        if (is_small()) {
            strcpy_s(data.static_str, sizeof(data.static_str), text); 
            data.dynamic.m_pointer = nullptr;
        }
        else {
            data.dynamic.m_size = len;
            data.dynamic.m_capacity = len;
            data.dynamic.m_pointer = new char[len + 1]; 
            strcpy_s(data.dynamic.m_pointer, len + 1, text);
        }
    }

    ~mystring() {
        if (!is_small() && data.dynamic.m_pointer)
            delete[] data.dynamic.m_pointer;
    }
};

int main() {
        char text[100];    
        std::cout << "Enter Text: ";
        std::cin.getline(text, sizeof(text));   

        mystring a(text);   
        a.print();         
        a.where();          
    }
