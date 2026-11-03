#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>
#include <vector>

template <typename T>
class CustomQueue {
private:
    struct QueueNode {
        T data;
        QueueNode* next;
        explicit QueueNode(const T& val) : data(val), next(nullptr) {}
    };

    QueueNode* frontNode;
    QueueNode* rearNode;
    size_t count;

public:
    CustomQueue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~CustomQueue() {
        clear();
    }

    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    void enqueue(const T& value) {
        QueueNode* newNode = new QueueNode(value);
        if (rearNode == nullptr) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("Queue underflow: empty queue.");
        }
        QueueNode* temp = frontNode;
        T val = temp->data;
        frontNode = frontNode->next;
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        count--;
        return val;
    }

    bool remove(const T& value) {
        QueueNode* curr = frontNode;
        QueueNode* prev = nullptr;
        while (curr) {
            if (curr->data == value) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    frontNode = curr->next;
                }
                if (curr == rearNode) {
                    rearNode = prev;
                }
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    T front() const {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty.");
        }
        return frontNode->data;
    }

    T rear() const {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty.");
        }
        return rearNode->data;
    }

    bool isEmpty() const { return frontNode == nullptr; }
    size_t size() const { return count; }

    std::vector<T> toVector() const {
        std::vector<T> vec;
        QueueNode* curr = frontNode;
        while (curr) {
            vec.push_back(curr->data);
            curr = curr->next;
        }
        return vec;
    }
};

#endif // QUEUE_H