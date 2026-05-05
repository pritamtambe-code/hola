#include <omp.h>
#include <iostream>
#include <chrono>
#include <cstdlib> // For random numbers

using namespace std::chrono;
using namespace std;

// ==========================================
// SEQUENTIAL OPERATIONS (Normal CPU)
// ==========================================
void seqOperations(int nums[], int length) {
    int minVal = nums[0];
    int maxVal = nums[0];
    long long sum = 0; // Use long long for massive arrays

    for (int i = 0; i < length; i++) {
        if (nums[i] < minVal) minVal = nums[i];
        if (nums[i] > maxVal) maxVal = nums[i];
        sum += nums[i];
    }
    
    cout << "Sequential Results -> Min: " << minVal 
         << ", Max: " << maxVal 
         << ", Sum: " << sum 
         << ", Avg: " << (double)sum / length << endl;
}

// ==========================================
// PARALLEL OPERATIONS (OpenMP)
// ==========================================
void parOperations(int nums[], int length) {
    int minVal = nums[0];
    int maxVal = nums[0];
    long long sum = 0;

    // We can do all reductions in one single parallel block!
    #pragma omp parallel for reduction(min: minVal) reduction(max: maxVal) reduction(+: sum)
    for (int i = 0; i < length; i++) {
        if (nums[i] < minVal) minVal = nums[i];
        if (nums[i] > maxVal) maxVal = nums[i];
        sum += nums[i];
    }

    cout << "Parallel Results   -> Min: " << minVal 
         << ", Max: " << maxVal 
         << ", Sum: " << sum 
         << ", Avg: " << (double)sum / length << endl;
}

// ==========================================
// MAIN FUNCTION
// ==========================================
int main() {
    // 1. Create a massive array to actually see the parallel speedup
    int length = 10000000; // 10 Million elements
    
    // Allocate memory dynamically because 10M is too big for the standard stack
    int* nums = new int[length]; 

    cout << "Generating array of " << length << " random numbers..." << endl;
    for (int i = 0; i < length; i++) {
        nums[i] = rand() % 1000; // Random numbers between 0 and 999
    }
    cout << "Array generated successfully!\n" << endl;

    // 2. Measure Sequential Time
    auto start_seq = high_resolution_clock::now();
    seqOperations(nums, length);
    auto stop_seq = high_resolution_clock::now();
    auto duration_seq = duration_cast<microseconds>(stop_seq - start_seq);

    // 3. Measure Parallel Time
    auto start_par = high_resolution_clock::now();
    parOperations(nums, length);
    auto stop_par = high_resolution_clock::now();
    auto duration_par = duration_cast<microseconds>(stop_par - start_par);

    // 4. Print the final time comparison
    cout << "\n--- PERFORMANCE COMPARISON ---" << endl;
    cout << "Sequential Execution Time: " << duration_seq.count() << " microseconds" << endl;
    cout << "Parallel Execution Time  : " << duration_par.count() << " microseconds" << endl;

    // Free the memory
    delete[] nums;

    return 0;
}