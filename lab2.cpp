#include <omp.h>
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm> // for std::copy

using namespace std::chrono;
using namespace std;

// --- UTILITY FUNCTIONS ---

void displayArray(string message, int nums[], int length) {
    cout << "\t" << message << ": [";
    for (int i = 0; i < length; i++) {
        cout << nums[i] << (i == length - 1 ? "" : ", ");
    }
    cout << "]" << endl;
}

void merge(int nums[], int leftStart, int leftEnd, int rightStart, int rightEnd) {
    int n = (rightEnd - leftStart) + 1;
    int* tempArray = new int[n];
    int t = 0, l = leftStart, r = rightStart;

    while (l <= leftEnd && r <= rightEnd) {
        if (nums[l] <= nums[r]) tempArray[t++] = nums[l++];
        else tempArray[t++] = nums[r++];
    }
    while (l <= leftEnd) tempArray[t++] = nums[l++];
    while (r <= rightEnd) tempArray[t++] = nums[r++];

    for (int i = 0; i < n; i++) nums[leftStart + i] = tempArray[i];
    delete[] tempArray;
}

// --- BUBBLE SORT METHODS ---

void bubbleSortLinear(int nums[], int length) {
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (nums[j] > nums[j + 1]) swap(nums[j], nums[j + 1]);
        }
    }
}

void bubbleSortParallel(int nums[], int length) {
    for (int i = 0; i < length; i++) {
        int start = i % 2; 
        #pragma omp parallel for
        for (int j = start; j < length - 1; j += 2) {
            if (nums[j] > nums[j + 1]) swap(nums[j], nums[j + 1]);
        }
    }
}

// --- MERGE SORT METHODS ---

void mergeSortLinear(int nums[], int start, int end) {
    if (start < end) {
        int mid = (start + end) / 2;
        mergeSortLinear(nums, start, mid);
        mergeSortLinear(nums, mid + 1, end);
        merge(nums, start, mid, mid + 1, end);
    }
}

void mergeSortParallel(int nums[], int start, int end) {
    if (start < end) {
        int mid = (start + end) / 2;
        #pragma omp parallel sections
        {
            #pragma omp section
            mergeSortParallel(nums, start, mid);
            #pragma omp section
            mergeSortParallel(nums, mid + 1, end);
        }
        merge(nums, start, mid, mid + 1, end);
    }
}

// --- MAIN ---

int main() {
    int original[] = {4, 6, 2, 0, 7, 6, 1, 9, -3, -5};
    int n = sizeof(original) / sizeof(int);
    int working[10];

    // --- BUBBLE SORT SECTION ---
    cout << "=== BUBBLE SORT COMPARISON ===" << endl;
    displayArray("Original Array", original, n);

    // Linear
    copy(begin(original), end(original), working);
    auto start = high_resolution_clock::now();
    bubbleSortLinear(working, n);
    auto stop = high_resolution_clock::now();
    displayArray("After Linear", working, n);
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " us\n" << endl;

    // Parallel
    copy(begin(original), end(original), working);
    start = high_resolution_clock::now();
    bubbleSortParallel(working, n);
    stop = high_resolution_clock::now();
    displayArray("After Parallel", working, n);
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " us\n" << endl;

    // --- MERGE SORT SECTION ---
    cout << "=== MERGE SORT COMPARISON ===" << endl;
    displayArray("Original Array", original, n);

    // Linear
    copy(begin(original), end(original), working);
    start = high_resolution_clock::now();
    mergeSortLinear(working, 0, n - 1);
    stop = high_resolution_clock::now();
    displayArray("After Linear", working, n);
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " us\n" << endl;

    // Parallel
    copy(begin(original), end(original), working);
    start = high_resolution_clock::now();
    mergeSortParallel(working, 0, n - 1);
    stop = high_resolution_clock::now();
    displayArray("After Parallel", working, n);
    cout << "Time: " << duration_cast<microseconds>(stop - start).count() << " us\n" << endl;

    return 0;
}