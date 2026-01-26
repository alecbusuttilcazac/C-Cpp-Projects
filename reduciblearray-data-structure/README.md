
# ReducibleArray C++/C Library

Flexible, safe, and efficient dynamic array for C++17 and C, with activeness tracking and robust memory management. This project provides a custom container for scenarios where you need to manage element lifetimes and reduce memory usage explicitly and efficiently, beyond what `std::vector` offers.

---
## About AI Assistance

This project was developed with the help of AI (GitHub Copilot). Nearly 100% of the code was written by me. AI did not generate code in large batches, but instead provided guidance, bug fixes, and documentation support throughout the process. Specifically, AI played a key role in:

- Guiding design decisions and API structure
- Fixing bugs and improving memory management
- Reviewing and refining code for safety and idiomatic C++
- Providing advice on error handling and best practices
- Generating and editing this README for clarity and completeness

The README itself was generated using AI, ensuring that documentation is clear, up-to-date, and accurately reflects the current state of the project. All code changes and documentation were reviewed and integrated interactively, with human oversight and iterative improvement.

## Overview
This README describes the `ReducibleArray` C++/C project. The C version is not yet complete and will not be uploaded until ready. For now, use the C++ implementation for all purposes.
This repository contains two versions of the `ReducibleArray` data structure:

- **C++ Version**: Fully implemented, robust, and tested. Provides a dynamic array with manual control over allocation, destruction, and activeness of elements. Designed for advanced use cases where you need to track which elements are "active" and safely manage memory.

## Files

- `ReducibleArray.hpp` — Main header for the C++ implementation. Contains the `ReducibleArray<T>` template class and all core logic.
- `ReducibleArrayUser.cpp` — Example and test file demonstrating usage, including construction, allocation, activeness checks, and error handling.
<!-- - `c/` — Experimental C implementation (not complete, not for upload):
	- `ReducibleArray.c`, `ReducibleArray.h`, `ReducibleArrayUser.c`, `BitpackedFlags.h` -->

## Key Features (C++ Version)

- Manual memory management: allocate, destroy, and clear elements explicitly.
- Activeness tracking: know which elements are "active" and prevent unsafe access.
- Safe access: operator[] throws on inactive access (const and non-const versions).
- Flexible resizing and reserving: control capacity and used count independently.
- Iterators for active and all elements.
- Exception-based error handling for robust code.

### Main API

- `ReducibleArray<T>::reserve(size_t)` — Reserve capacity for elements.
- `ReducibleArray<T>::resize(size_t, const T&)` — Resize and fill with default or provided value.
- `ReducibleArray<T>::emplace(size_t, Args&&...)` — Construct element in-place at index.
- `ReducibleArray<T>::destroy(size_t)` — Destroy element at index and mark inactive.
- `ReducibleArray<T>::clear()` — Destroy all elements and reset activeness.
- `ReducibleArray<T>::operator[](size_t)` — Access element by index (throws if inactive).
- Iterators: `begin()`, `end()`, `active_begin()`, `active_end()`.

## Build and Run

From the `reduciblearray-data-structure` directory, compile the example with g++ (C++17 or newer):

```bash
g++ -std=c++17 ReducibleArrayUser.cpp -o bin/ReducibleArrayUser
./bin/ReducibleArrayUser
```

## Limitations and TODOs

- The C version is incomplete and not ready for use. It will not be uploaded until finished and tested.
- The C++ version is robust but may be further improved with more tests and API refinements.

## License

See repository root `LICENSE` for licensing information.

---
This README describes the `ReducibleArray` C++/C project. The C version is not yet complete and will not be uploaded until ready. For now, use the C++ implementation for all purposes.

