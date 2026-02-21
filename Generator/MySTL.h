#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

template <typename T>
class MyVector {
    T* data;
    int capacity;
    int length;
public:
    MyVector(int initialCapacity = 10) {
        capacity = initialCapacity;
        length = 0;
        data = new T[capacity];
    }
    MyVector(const MyVector& other) {
        capacity = other.capacity;
        length = other.length;
        data = new T[capacity];
        for (int i = 0; i < length; i++) {
            data[i] = other.data[i];
        }
    }
    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            length = other.length;
            data = new T[capacity];
            for (int i = 0; i < length; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    ~MyVector() {
        delete[] data;
    }

    void push_back(const T& element) {
        if (length >= capacity) {
            capacity *= 2;
            T* newData = new T[capacity];
            for (int i = 0; i < length; i++) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
        }
        data[length++] = element;
    }
    T& back() {
        if (length > 0) {
            return data[length - 1];
        }
        throw "Vector is empty";
    }
    void pop_back() {
        if (length > 0) {
            length--;
        }
    }
    T& operator[](int index) {
        if (index >= 0 && index < length) {
            return data[index];
        }
        throw "索引越界";
    }

    const T& operator[](int index) const {
        if (index >= 0 && index < length) {
            return data[index];
        }
        throw "索引越界";
    }

    int size() const {
        return length;
    }

    bool empty() const {
        return length == 0;
    }

    void clear() {
        length = 0;
    }

    void resize(int newSize) {
        if (newSize <= capacity) {
            length = newSize;
        }
        else {
            capacity = newSize;
            T* newData = new T[capacity];
            for (int i = 0; i < length; i++) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
            length = newSize;
        }
    }
};

template <typename T>
class MyStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& data) : data(data), next(nullptr) {}
    };
    Node* topNode;
    int size;

public:
    MyStack() : topNode(nullptr), size(0) {}
    ~MyStack() {
        while (!empty()) {
            pop();
        }
    }

    void push(const T& data) {
        Node* newNode = new Node(data);
        newNode->next = topNode;
        topNode = newNode;
        size++;
    }

    void pop() {
        if (!empty()) {
            Node* temp = topNode;
            topNode = topNode->next;
            delete temp;
            size--;
        }
    }
    T& top() {
        if (!empty()) {
            return topNode->data;
        }
        throw "Stack is empty";
    }
    bool empty() const {
        return topNode == nullptr;
    }
    int getSize() const {
        return size;
    }
};