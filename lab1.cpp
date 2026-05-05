#include <omp.h>
#include <iostream>
#include <queue>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Node {
public:
    int value;
    Node *left, *right;
    Node(int val) : value(val), left(nullptr), right(nullptr) {}
};

// --- Tree Generation ---
Node* generateTree(int values[], int length) {
    if (length == 0) return nullptr;
    Node** nodes = new Node*[length];
    for (int i = 0; i < length; i++) {
        nodes[i] = (values[i] != -1) ? new Node(values[i]) : nullptr;
    }
    int child = 1;
    for (int i = 0; child < length; i++) {
        if (nodes[i]) {
            nodes[i]->left = nodes[child++];
            if (child < length) nodes[i]->right = nodes[child++];
        }
    }
    return nodes[0];
}

// --- Sequential BFS ---
void bfs(Node* root) {
    if (!root) return;
    queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* node = q.front();
        q.pop();
        cout << node->value << " ";
        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }
}

// --- Parallel DFS (Using Tasks) ---
void dfsParallel(Node* root) {
    if (!root) return;

    // Use critical to prevent threads from "printing over" each other
    #pragma omp critical
    cout << root->value << " ";

    // We create "tasks" for children. 
    // This is more efficient than "sections" for recursive structures.
    #pragma omp task
    dfsParallel(root->left);

    #pragma omp task
    dfsParallel(root->right);
}

int main() {
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int length = sizeof(values) / sizeof(int);
    Node* root = generateTree(values, length);

    // BFS Execution
    cout << "BFS (Sequential): ";
    auto s1 = high_resolution_clock::now();
    bfs(root);
    auto e1 = high_resolution_clock::now();
    cout << "\nTime: " << duration_cast<microseconds>(e1 - s1).count() << " us\n\n";

    // DFS Execution
    cout << "DFS (Parallel Tasks): ";
    auto s2 = high_resolution_clock::now();
    
    #pragma omp parallel
    {
        // One thread starts the recursion; others pick up the tasks
        #pragma omp single
        dfsParallel(root);
    }
    
    auto e2 = high_resolution_clock::now();
    cout << "\nTime: " << duration_cast<microseconds>(e2 - s2).count() << " us" << endl;

    return 0;
}