#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <vector>
#include <utility>
#include <cstddef>

template <typename K, typename V>
class HashTable {
private:
    struct HashNode {
        K key;
        V value;
        HashNode* next;
        HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    std::vector<HashNode*> buckets;
    size_t numBuckets;
    size_t elementCount;

    size_t hashFunction(const std::string& key) const {
        size_t hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + static_cast<size_t>(c);
        }
        return hash % numBuckets;
    }

public:
    explicit HashTable(size_t capacity = 16) : numBuckets(capacity), elementCount(0) {
        buckets.resize(numBuckets, nullptr);
    }

    ~HashTable() {
        clear();
    }

    void clear() {
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr) {
                HashNode* prev = curr;
                curr = curr->next;
                delete prev;
            }
            buckets[i] = nullptr;
        }
        elementCount = 0;
    }

    bool insert(const K& key, const V& value) {
        size_t idx = hashFunction(key);
        HashNode* curr = buckets[idx];
        while (curr) {
            if (curr->key == key) {
                curr->value = value;
                return false;
            }
            curr = curr->next;
        }
        HashNode* newNode = new HashNode(key, value);
        newNode->next = buckets[idx];
        buckets[idx] = newNode;
        elementCount++;
        return true;
    }

    bool search(const K& key, V& outValue) const {
        size_t idx = hashFunction(key);
        HashNode* curr = buckets[idx];
        while (curr) {
            if (curr->key == key) {
                outValue = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool remove(const K& key) {
        size_t idx = hashFunction(key);
        HashNode* curr = buckets[idx];
        HashNode* prev = nullptr;
        while (curr) {
            if (curr->key == key) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    buckets[idx] = curr->next;
                }
                delete curr;
                elementCount--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    size_t size() const { return elementCount; }
    size_t getCapacity() const { return numBuckets; }

    std::vector<std::vector<std::pair<K, V>>> getBucketSnapshot() const {
        std::vector<std::vector<std::pair<K, V>>> snapshot(numBuckets);
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr) {
                snapshot[i].push_back({curr->key, curr->value});
                curr = curr->next;
            }
        }
        return snapshot;
    }
};

#endif // HASHTABLE_H