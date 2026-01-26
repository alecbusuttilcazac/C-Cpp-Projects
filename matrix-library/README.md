# Matrix Library (C++)

A lightweight, header-only C++ library for working with 2D matrices. This project provides basic matrix operations and utilities for educational and prototyping purposes.

This is not a complete or production-ready project. The library is designed to work only with 2D matrices. It is suitable for simple use cases, learning, and experimentation.

---
## About AI Assistance

This library was written almost entirely by me. AI (GitHub Copilot) provided occasional (mathematical) guidance, bug fixes, and documentation suggestions, but did not generate code in large batches.

## Files

- `Matrix.hpp` — Main header file containing the `Matrix` class and related functions for 2D matrix operations.
- `MatrixUser.cpp` — Example and test file demonstrating usage of the matrix library.
- `include/Error.hpp` — Error handling utilities.

## Key Features

- Simple 2D matrix class using `std::vector<std::vector<T>>` internally.
- Basic operations: construction, element access, resizing, and assignment.
- Example usage and tests provided in `MatrixUser.cpp`.
- Header-only, requires C++17 or newer.

## Limitations

- Only supports 2D matrices (no tensors or higher-dimensional arrays).
- Not optimized for performance or large-scale numerical computing.
- No advanced linear algebra (eigenvalues, decomposition, etc.).
- Not a complete or polished library—intended for learning and prototyping.

## Usage Example

```cpp
#include "Matrix.hpp"

Matrix<int> mat(3, 3); // 3x3 integer matrix
mat(0, 0) = 1;
mat(1, 1) = 2;
mat(2, 2) = 3;

for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
        std::cout << mat(i, j) << ' ';
    }
    std::cout << std::endl;
}
```

See `MatrixUser.cpp` for more examples and tests.

## Build and Run

From the `matrix-library` directory, compile the example with g++ (C++17 or newer):

```bash
g++ -std=c++17 MatrixUser.cpp -o bin/MatrixUser
./bin/MatrixUser
```

## License

See repository root `LICENSE` for licensing information.

---
This is a minimal, educational matrix library for 2D vectors in C++. It is not a complete project and is not intended for production use.