#include <iostream>

struct Node {
    int data;
    Node* prev;
    Node* next;

    Node(int value) {
        data = value;
        prev = nullptr;
        next = nullptr;
    }
};

class List {
private:
    Node* head;
    Node* tail;
    int size;

public:
    List() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    ~List() {
        while (head) {
            pop_front();
        }
    }

    void push_back(int value) {
        Node* newNode = new Node(value);
        if (!tail) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }

    void push_front(int value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        }
        else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        size++;
    }

    void pop_back() {
        if (!tail) return;
        Node* temp = tail;
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else head = nullptr;
        delete temp;
        size--;
    }

    void pop_front() {
        if (!head) return;
        Node* temp = head;
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;
        delete temp;
        size--;
    }

    bool empty() const {
        return size == 0;
    }

    int get_size() const {
        return size;
    }

    void print_forward() const {
        Node* current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    void print_backward() const {
        Node* current = tail;
        while (current) {
            std::cout << current->data << " ";
            current = current->prev;
        }
        std::cout << std::endl;
    }
};

int main() {
    List myList;
    myList.push_back(10);
    myList.push_front(5);
    myList.push_back(20);

    std::cout << "Forward: ";
    myList.print_forward();   

    std::cout << "Backward: ";
    myList.print_backward();  

    myList.pop_front();
    std::cout << "After pop_front: ";
    myList.print_forward();  

    myList.pop_back();
    std::cout << "After pop_back: ";
    myList.print_forward();  

    return 0;
}
