# Binary Search Tree

Production-ready header-only BST with duplicate tracking, balance detection, and auto-cleanup.


## About AI Assistance

This project was developed with the help of AI (GitHub Copilot). Nearly 100% of the code was written by me. AI did not generate code in large batches, but instead provided guidance, bug fixes, and documentation support throughout the process. Specifically, AI played a key role in:

- Guiding design decisions and API structure
- Fixing bugs and improving memory management
- Reviewing and refining code for safety and idiomatic C++
- Providing advice on error handling and best practices
- Generating and editing the majority of this README for clarity and completeness

## Overview

Header-only C++17 BST template with automatic memory management, duplicate tracking, balance checking, and three traversal orders. No manual cleanup required; compile-time generic with custom comparator support.

## Files

- `BinarySearchTree.hpp` — Implementation
- `Demo.cpp` — Test suite + benchmarks
- `include/Stack.hpp` — Unused

## Operations
- **Insert/Search/Remove** — O(log n) avg; handle duplicates via per-node count
- **Height** — O(n); on-demand (not cached)
- **Is Balanced** — O(n²); AVL validation
- **Min/Max** — O(log n); throws if empty
- **Size/Unique Size** — O(1)
- **Clear** — O(n); auto-cleanup via `unique_ptr`

### Traversals (return `std::vector<T>`)
- **Inorder** — O(n); sorted output
- **Preorder** — O(n); root-first
- **Postorder** — O(n); children-first

### Memory Management
- Auto-recursive cleanup via `unique_ptr` (no manual `delete`)
- Move semantics; copy semantics deleted

## API Reference

### Constructors
```cpp
BST()                                    // Default empty tree
BST(std::initializer_list<T>)           // Initialize from list
BST(const std::vector<T>&)              // Initialize from vector (copy)
BST(std::vector<T>&&)                   // Initialize from vector (move)
```

### Public Methods
```cpp
// Modification
void insert(const T& value)             // Add or increment count
bool remove(const T& value)             // Delete or decrement count
void clear()                            // Destroy all elements

// Queries
uint32_t size() const                   // Total size (with duplicates)
uint32_t uniqueSize() const             // Unique value count
bool isEmpty() const                    // Check if tree is empty
bool isLeaf() const                     // Check if root is leaf
int32_t height() const                  // Tree height (-1 for empty)
bool isBalanced() const                 // AVL balance check

// Search & Access
uint32_t search(const T& value) const   // Get count of value
bool contains(const T& value) const     // Check existence
const T& min() const                    // Minimum value (throws if empty)
const T& max() const                    // Maximum value (throws if empty)

// Traversals
std::vector<T> inorder() const          // Left-Root-Right (sorted)
std::vector<T> preorder() const         // Root-Left-Right
std::vector<T> postorder() const        // Left-Right-Root
```

### Test Coverage
✅ Insertions, duplicates, size tracking  
✅ Height, balance detection  
✅ Min/max, search, contains  
✅ All three traversals (sorted, pre/post order)  
✅ Removal (duplicates, leaves, internal nodes)  
✅ Clear, empty state  
✅ Performance: ~84 μs full suite

## Design

**Duplicates**: Per-node `count` + `totalCount` (total) + `uniqueCount` (unique). Remove decrements before deletion.

**Memory**: `unique_ptr<BST_Node<T>>` with auto-cascade cleanup. No manual `delete`.

**Comparators**: `template<typename T, typename Comparator = std::less<T>>`

**Safety**: All queries `const`. Throws on invalid access (empty tree). No copy semantics.

## Limitations

- Not self-balancing (can degenerate to O(n) on sorted input)
- Balance checking is O(n²) (no height caching)
- Traversals return vectors (no custom iterators)
- No range queries

## License

See root `LICENSE`
