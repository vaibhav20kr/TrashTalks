#ifndef MAXHEAP_H
#define MAXHEAP_H

#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

struct HeapNode {
    std::string binId;
    std::string location;
    double priorityScore;
    double fillLevel;
    double hoursAgo;
    double riskFactor;
};

class MaxHeap {
private:
    std::vector<HeapNode> heap;

    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (heap[index].priorityScore > heap[parent].priorityScore) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(size_t index) {
        size_t size = heap.size();
        while (index < size) {
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            size_t largest = index;

            if (left < size && heap[left].priorityScore > heap[largest].priorityScore) {
                largest = left;
            }
            if (right < size && heap[right].priorityScore > heap[largest].priorityScore) {
                largest = right;
            }

            if (largest != index) {
                std::swap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    MaxHeap() = default;

    void clear() { heap.clear(); }

    void insert(const HeapNode& node) {
        heap.push_back(node);
        heapifyUp(heap.size() - 1);
    }

    HeapNode extractMax() {
        if (heap.empty()) {
            throw std::runtime_error("Heap underflow: empty priority queue.");
        }
        HeapNode maxNode = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return maxNode;
    }

    HeapNode peek() const {
        if (heap.empty()) {
            throw std::runtime_error("Heap is empty.");
        }
        return heap[0];
    }

    size_t size() const { return heap.size(); }
    bool isEmpty() const { return heap.empty(); }

    const std::vector<HeapNode>& getElements() const { return heap; }
};

#endif // MAXHEAP_H