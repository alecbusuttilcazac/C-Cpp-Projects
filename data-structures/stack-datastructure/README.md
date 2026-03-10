# Stack (C++)

Lightweight generic stack template with LIFO operations, deep copy semantics, and exception safety.

## About AI Assistance

This project was developed with the help of AI (GitHub Copilot). Nearly 100% of the code was written by me. AI did not generate code in large batches, but instead provided guidance, bug fixes, and documentation support throughout the process. Specifically, AI played a key role in:

- Guiding design decisions and API structure
- Fixing bugs and improving memory management
- Reviewing and refining code for safety and idiomatic C++
- Providing advice on error handling and best practices
- Generating and editing the majority of this README for clarity and completeness

## Files

- `Stack.hpp` — Header-only implementation
- `Demo.cpp` — Usage examples + tests

## Operations

- **Push** — O(1) amortized; add element
- **Pop** — O(1); remove & return top (throws if empty)
- **Peek** — O(1); view top without removing (throws if empty)
- **Size** — O(1)
- **IsEmpty** — O(1)
- **Clear** — O(n); remove all
- **Reserve** — O(n); pre-allocate capacity

## API

```cpp
Stack<T>()                      // Default
Stack<T>(size_t initialSize)    // Pre-reserve
Stack<T>(const Stack&)          // Copy (deep)
Stack<T>& operator=(const Stack&)

void push(const T&)             // Add to top
T pop()                         // Remove & return top
const T& peek() const           // View top
bool isEmpty() const noexcept   
size_t size() const noexcept    
void reserve(size_t)            
void clear() noexcept           
```

## Build

```bash
g++ -std=c++17 -O3 Demo.cpp -o bin/Demo
./bin/Demo
```

## Design

- Backed by `std::vector` for dynamic allocation
- Copy constructor performs deep copy
- Exceptions on underflow (`pop`/`peek` on empty)
- `noexcept` on safe operations (isEmpty, size, clear)

## License

See root `LICENSE`
