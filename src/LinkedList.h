#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <vector>
#include <functional>
#include <cstddef>

template <typename T>
class CustomLinkedList {
private:
    struct ListNode {
        T data;
        ListNode* next;
        explicit ListNode(const T& val) : data(val), next(nullptr) {}
    };

    ListNode* head;
    size_t count;

public:
    CustomLinkedList() : head(nullptr), count(0) {}

    ~CustomLinkedList() {
        clear();
    }

    void clear() {
        ListNode* curr = head;
        while (curr) {
            ListNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = nullptr;
        count = 0;
    }

    void insertFront(const T& value) {
        ListNode* newNode = new ListNode(value);
        newNode->next = head;
        head = newNode;
        count++;
    }

    void insertBack(const T& value) {
        ListNode* newNode = new ListNode(value);
        if (!head) {
            head = newNode;
        } else {
            ListNode* curr = head;
            while (curr->next) {
                curr = curr->next;
            }
            curr->next = newNode;
        }
        count++;
    }

    bool remove(std::function<bool(const T&)> predicate) {
        ListNode* curr = head;
        ListNode* prev = nullptr;

        while (curr) {
            if (predicate(curr->data)) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    head = curr->next;
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

    std::vector<T> search(std::function<bool(const T&)> predicate) const {
        std::vector<T> results;
        ListNode* curr = head;
        while (curr) {
            if (predicate(curr->data)) {
                results.push_back(curr->data);
            }
            curr = curr->next;
        }
        return results;
    }

    std::vector<T> toVector() const {
        std::vector<T> vec;
        ListNode* curr = head;
        while (curr) {
            vec.push_back(curr->data);
            curr = curr->next;
        }
        return vec;
    }

    size_t size() const { return count; }
    bool isEmpty() const { return head == nullptr; }
};

#endif // LINKEDLIST_H