# DTE-3611 Algorithms - Design and Analysis

Master's coursework - UiT The Arctic University of Norway, Fall 2025 (Grade: A)

## Introduction

This repository presents a comparative study of fundamental algorithms across multiple families: sorting, string matching, graph traversal, shortest path computation, dynamic programming for NP-complete problems, and network flow. All algorithms are implemented in C++20, leveraging the Boost Graph Library for graph representations. The objective is twofold: to validate theoretical complexity bounds through empirical measurement, and to identify practical performance characteristics under various conditions.

## Repository Structure

```
lib3611/include/lib3611/    - Algorithm implementations (header-only)

benchmark_and_testing/
    unittests/              - Google Test suites
    benchmarks/             - Google Benchmark suites
    
whitepaper/                 - Technical article (PDF + LaTeX source)
```

## Algorithms Implemented

### Sorting Algorithms

**Binary sort** employs a self-balancing binary search tree (red-black tree via `std::multiset`) for element insertion, followed by in-order traversal extraction. The BST property guarantees O(log n) insertion, yielding O(n log n) total complexity.

**Counting sort** operates in O(n + k) time where k denotes the value range. The algorithm constructs a histogram, computes prefix sums, then places elements in stable order. This achieves linear time for bounded integers but requires O(n + k) auxiliary space.

**Radix sort** processes elements digit by digit, applying counting sort at each position. For d-digit numbers in base b, complexity is O(d(n + b)). The implementation processes bytes (b = 256), yielding linear time for fixed-width integers.

**Hybrid quicksort** combines non-recursive quicksort with insertion sort for small subarrays. It employs median-of-three pivot selection and Hoare partitioning, achieving expected O(n log n) complexity.

### String Matching

**Naive search** slides a window across the text, performing character comparisons at each position. Worst-case complexity reaches O(nm) for text length n and pattern length m.

**Knuth-Morris-Pratt** preprocesses the pattern to construct a failure function (LPS table) indicating optimal shift distances upon mismatch. Preprocessing requires O(m) time, while the search phase completes in O(n), yielding O(n + m) total complexity.

### Graph Traversal

**Breadth-first search (BFS)** explores vertices level by level using a queue data structure. When a vertex is dequeued, its distance from the source is definitively established. Complexity is O(V + E).

**Depth-first search (DFS)** explores as deeply as possible before backtracking, using a stack (explicit or via recursion). Upon completion of a vertex's processing, all reachable descendants have been visited. Complexity is O(V + E).

### Shortest Path

The implementation provides a **unified Dijkstra/A* framework** through a generic priority function abstraction. Dijkstra uses g(v) as priority, while A* employs g(v) + h(v) where h is an admissible heuristic.

**Dijkstra's algorithm** maintains tentative distances and iteratively extracts minimum-distance vertices from a priority queue. With a binary heap, complexity is O((V + E) log V). The algorithm requires non-negative edge weights.

**A* search** extends Dijkstra by incorporating a heuristic function estimating remaining distance to the goal. With admissible and consistent heuristics, A* guarantees optimality while potentially exploring fewer vertices.

### Dynamic Programming for NP-Complete Problems

**Subset-sum** determines whether a subset of n integers sums to a target W. The DP formulation defines Boolean subproblems DP[i][w] indicating achievability, with recurrence capturing inclusion/exclusion choices. Complexity is O(n * W), pseudo-polynomial compared to O(2^n) brute-force enumeration.

**0/1 Knapsack** maximises total value of selected items subject to a weight constraint. The Bellman recurrence OPT(i, w) considers three cases: base condition, item exceeding capacity, and the fundamental include/exclude choice. The implementation includes solution reconstruction via backtracking. Complexity is O(n * W).

The theoretical analysis includes **NP-completeness proofs** via the reduction chain 3-SAT to Subset-Sum to Knapsack, demonstrating that no polynomial-time algorithm exists under standard complexity assumptions.

### Network Flow

**Edmonds-Karp** implements the Ford-Fulkerson method using BFS for augmenting path discovery, guaranteeing selection of shortest paths in terms of edge count. Worst-case complexity is O(V * E^2).

**Bellman-Ford** computes single-source shortest paths while tolerating negative edge weights. The algorithm performs |V| - 1 relaxation iterations over all edges, with a subsequent pass for negative cycle detection. Complexity is O(V * E).

**Cycle-canceling** solves the minimum-cost maximum flow problem. The algorithm first computes a maximum flow, then iteratively detects and eliminates negative-cost cycles in the residual graph using Bellman-Ford traversals.

**Bipartite matching** is solved via reduction to maximum flow: construct a flow network with source connected to one partition, sink connected to the other, and unit capacities on all edges. The maximum flow value equals the maximum matching cardinality.

### Amortized Analysis and Data Structures

The study examines **std::vector::push_back** amortization, validating that the doubling strategy yields O(1) amortized insertion cost despite occasional O(n) reallocations.

**Dictionary comparison** contrasts std::map (red-black tree, O(log n) worst-case) against std::unordered_map (hash table, O(1) expected, O(n) worst-case).

**Polymorphic Memory Resources (PMR)** evaluation compares three allocation strategies: standard allocator (general-purpose via malloc/free), monotonic buffer resource (bump pointer allocation), and unsynchronised pool resource (fixed-size block pools).

### Linear Programming (Theoretical)

The single-source shortest path problem admits a natural LP formulation. Decision variables d_v represent distances, with constraints encoding the triangle inequality. Dijkstra's algorithm solves this LP implicitly by exploiting combinatorial structure, demonstrating that classical graph algorithms can be viewed as specialised LP solvers.

## Technical Paper

Detailed benchmark results, performance analysis, and theoretical discussion are presented in the accompanying [technical article](./whitepaper/DTE-3611-1-25H-CALDEIRA-RIBEIRO-DIOGO-WEEK3-4.pdf).

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

Requirements: CMake 3.16+, C++20 compiler, Boost Graph Library

---

Diogo Caldeira Ribeiro  
DTE-3611 Algorithms - Design and Analysis  
UiT The Arctic University of Norway