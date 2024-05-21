#ifndef _queue_hpp_
#define _queue_hpp_

#include <stdexcept>

#define MAX_QUEUE_ELEMENTS 256

template<typename T>
class Queue {
private:
    T array[MAX_QUEUE_ELEMENTS];
    int head;
    int tail;
public:
    Queue () {
        this->head = 0;
        this->tail = 0;
    }
    void push (T a) {
        this->array[this->tail] = a;
        this->tail = (this->tail + 1) % MAX_QUEUE_ELEMENTS;
    };
    T shift () {
        if (this->isEmpty()) throw std::logic_error("shift empty queue\n");
        T element = this->array[this->head];
        this->head = (this->head + 1) % MAX_QUEUE_ELEMENTS;
        return element;
    };
    bool isEmpty () { return this->head == this->tail; };
};

#endif
