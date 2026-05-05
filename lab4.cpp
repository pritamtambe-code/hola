#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cstdlib> // For rand()

using namespace std;
using namespace std::chrono;

// Standard partition function used by both sequential and parallel algorithms
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // Choosing the last element as the pivot
    int i = (low - 1);     // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// ==========================================
// 1. SEQUENTIAL QUICKSORT
// ==========================================
void sequentialQuickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        // Sort the elements separately before and after partition
        sequentialQuickSort(arr, low, pi - 1);
        sequentialQuickSort(arr, pi + 1, high);
    }
}

// ==========================================
// 2. ENHANCED PARALLEL QUICKSORT
// ==========================================
void parallelQuickSort(int arr[], int low, int high) {
    if (low < high) {
        // PERFORMANCE ENHANCEMENT: Threshold Check
        // If the array size is less than 1000, don't waste time making threads.
        if (high - low < 1000) {
            sequentialQuickSort(arr, low, high);
        } 
        else {
            int pi = partition(arr, low, high);

            // Create independent tasks for the left and right subarrays
            #pragma omp task shared(arr)
            parallelQuickSort(arr, low, pi - 1);

            #pragma omp task shared(arr)
            parallelQuickSort(arr, pi + 1, high);

            // Wait for both subarrays to finish sorting before returning
            #pragma omp taskwait
        }
    }
}

// ==========================================
// MAIN FUNCTION
// ==========================================
int main() {
    int n = 5000000; // 5 Million elements to clearly show speedup
    
    // Dynamically allocate memory to prevent stack overflow
    int* arr_seq = new int[n];
    int* arr_par = new int[n];

    cout << "Generating array of " << n << " random numbers..." << endl;
    for (int i = 0; i < n; i++) {
        int val = rand() % 10000;
        arr_seq[i] = val;
        arr_par[i] = val; // Exact same array for fair comparison
    }
    cout << "Array generated successfully!\n" << endl;

    // --- SEQUENTIAL EXECUTION ---
    auto start_seq = high_resolution_clock::now();
    sequentialQuickSort(arr_seq, 0, n - 1);
    auto stop_seq = high_resolution_clock::now();
    auto duration_seq = duration_cast<milliseconds>(stop_seq - start_seq);

    // --- PARALLEL EXECUTION ---
    auto start_par = high_resolution_clock::now();
    
    // We must open a parallel region, and use 'single' so only one 
    // master thread kicks off the initial recursion task.
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            parallelQuickSort(arr_par, 0, n - 1);
        }
    }
    
    auto stop_par = high_resolution_clock::now();
    auto duration_par = duration_cast<milliseconds>(stop_par - start_par);

    // --- PERFORMANCE COMPARISON ---
    cout << "--- PERFORMANCE ENHANCEMENT RESULTS ---" << endl;
    cout << "Sequential Time : " << duration_seq.count() << " milliseconds" << endl;
    cout << "Parallel Time   : " << duration_par.count() << " milliseconds" << endl;
    
    // Free the memory
    delete[] arr_seq;
    delete[] arr_par;

    return 0;
}