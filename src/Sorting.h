#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

struct SortMetrics {
    long long comparisons = 0;
    long long swaps = 0;
    long long movements = 0;
    double executionTimeMs = 0.0;
};

struct SortStep {
    std::vector<int> data;
    int activeIdx1 = -1;
    int activeIdx2 = -1;
    std::string description;
};

class SortingVisualizer {
public:
    static void bubbleSort(std::vector<int> arr, SortMetrics& metrics, std::vector<SortStep>& steps) {
        metrics = SortMetrics{};
        steps.clear();
        auto start = std::chrono::high_resolution_clock::now();

        int n = static_cast<int>(arr.size());
        steps.push_back({arr, -1, -1, "Initial state"});

        for (int i = 0; i < n - 1; ++i) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; ++j) {
                metrics.comparisons++;
                steps.push_back({arr, j, j + 1, "Comparing index " + std::to_string(j) + " and " + std::to_string(j + 1)});

                if (arr[j] > arr[j + 1]) {
                    std::swap(arr[j], arr[j + 1]);
                    metrics.swaps++;
                    metrics.movements += 2;
                    swapped = true;
                    steps.push_back({arr, j, j + 1, "Swapped elements at " + std::to_string(j) + " & " + std::to_string(j + 1)});
                }
            }
            if (!swapped) break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        metrics.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        steps.push_back({arr, -1, -1, "Sorting completed"});
    }

    static void selectionSort(std::vector<int> arr, SortMetrics& metrics, std::vector<SortStep>& steps) {
        metrics = SortMetrics{};
        steps.clear();
        auto start = std::chrono::high_resolution_clock::now();

        int n = static_cast<int>(arr.size());
        steps.push_back({arr, -1, -1, "Initial state"});

        for (int i = 0; i < n - 1; ++i) {
            int minIdx = i;
            for (int j = i + 1; j < n; ++j) {
                metrics.comparisons++;
                steps.push_back({arr, minIdx, j, "Comparing min item at " + std::to_string(minIdx) + " with " + std::to_string(j)});
                if (arr[j] < arr[minIdx]) {
                    minIdx = j;
                }
            }
            if (minIdx != i) {
                std::swap(arr[i], arr[minIdx]);
                metrics.swaps++;
                metrics.movements += 2;
                steps.push_back({arr, i, minIdx, "Swapped min element to index " + std::to_string(i)});
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        metrics.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        steps.push_back({arr, -1, -1, "Sorting completed"});
    }

    static void mergeSort(std::vector<int> arr, SortMetrics& metrics, std::vector<SortStep>& steps) {
        metrics = SortMetrics{};
        steps.clear();
        auto start = std::chrono::high_resolution_clock::now();

        steps.push_back({arr, -1, -1, "Initial state"});
        mergeSortRecursive(arr, 0, static_cast<int>(arr.size()) - 1, metrics, steps);

        auto end = std::chrono::high_resolution_clock::now();
        metrics.executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        steps.push_back({arr, -1, -1, "Sorting completed"});
    }

private:
    static void mergeSortRecursive(std::vector<int>& arr, int left, int right, SortMetrics& metrics, std::vector<SortStep>& steps) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortRecursive(arr, left, mid, metrics, steps);
        mergeSortRecursive(arr, mid + 1, right, metrics, steps);
        merge(arr, left, mid, right, metrics, steps);
    }

    static void merge(std::vector<int>& arr, int left, int mid, int right, SortMetrics& metrics, std::vector<SortStep>& steps) {
        std::vector<int> L(arr.begin() + left, arr.begin() + mid + 1);
        std::vector<int> R(arr.begin() + mid + 1, arr.begin() + right + 1);

        int i = 0, j = 0, k = left;
        while (i < static_cast<int>(L.size()) && j < static_cast<int>(R.size())) {
            metrics.comparisons++;
            steps.push_back({arr, left + i, mid + 1 + j, "Merging subarrays"});
            if (L[i] <= R[j]) {
                arr[k] = L[i++];
            } else {
                arr[k] = R[j++];
            }
            metrics.movements++;
            k++;
        }

        while (i < static_cast<int>(L.size())) {
            arr[k++] = L[i++];
            metrics.movements++;
        }
        while (j < static_cast<int>(R.size())) {
            arr[k++] = R[j++];
            metrics.movements++;
        }
        steps.push_back({arr, left, right, "Merged segment [" + std::to_string(left) + ".." + std::to_string(right) + "]"});
    }
};

#endif // SORTING_H