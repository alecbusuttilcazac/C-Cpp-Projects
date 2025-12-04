
# cpp-table-library

Header-only C++17 Table utilities for small in-memory 2D tables (rows of string fields).

## Overview

This lightweight header-only library (`Table.hpp`) provides a convenient 2D table abstraction focused on CSV-like data: rows are `std::vector<std::string>` and the table is a `std::vector` of rows. The API emphasizes safety (checked access) and ergonomics (operator[] row proxies and iterator support).

Key features:

- `table::Table` — 2D table storing `std::vector<std::vector<std::string>>` internally.
- Checked accessors: `at(row,col)` throws on out-of-bounds, `operator[]` returns a proxy that performs checked row access and delegates column access to `at()`.
- Zero-copy read access: `view()` returns a `const&` to the internal data; `copy()` returns a deep copy.
- Explicit mutation APIs: `rowRef(row)` returns a mutable `std::vector<std::string>&` when you need in-place changes.
- Iterator support: `begin()`/`end()`/`cbegin()`/`cend()` so you can use range-for: `for (auto &row : table) { ... }`.
- Small, precise exception types: `RowOutOfBoundsException`, `ColumnOutOfBoundsException`, `NoTableHeaderException`.

The code requires C++17 or newer.

## Files

- `Table.hpp` — main header; contains `table::Table` and related exception types.
- `TableUser.cpp` — small example showing `operator[]`, `rowRef`, insertion/removal and header usage.

## Example usage

Create a table, set a header, read and mutate rows using `[]` and `[][]`:

```cpp
using namespace table;
Table t;
t.insertRow({"id","name","score"});
t.insertRow({"1","Alice","87"});
t.insertRow({"2","Bob","91"});

// Use header
t.setHeader(0);
t.removeRow(0);

// Iterate rows using range-for (iterators are provided)
for (const auto &row : t) {
	for (const auto &cell : row) std::cout << cell << ',';
	std::cout << '\n';
}

// Access with [][] (proxy then column)
std::string bobScore = t[1][2]; // row 1, col 2

// Mutate in-place using rowRef
auto &r = t.rowRef(1);
r[2] = "95"; // update Bob's score
```

Notes:

- `operator[]` returns a `RowProxy`. Binding the proxy by value (auto row = table[i]) preserves the proxy's non-const interface so you can call `row[col]`. If you need a real reference to the underlying vector, use `rowRef(i)`.
- `at(row,col)` performs checked access and throws a `RowOutOfBoundsException` or `ColumnOutOfBoundsException` on invalid indices.

## Build and run (quick)

From the `cpp-table-library` directory you can compile the example with g++ (C++17):

```bash
g++ -std=c++17 TableUser.cpp -I. -o bin/TableUser
./bin/TableUser
```

## Contributing and TODOs

- Consider making the `operator[]` proxies safer (remove implicit conversions to `std::vector&` and provide explicit `replaceRow()`/`row()` copy methods).
- Add unit tests for edge cases (empty table, ragged rows, header mismatches).
- Document thread-safety (table is not thread-safe; external synchronization required for concurrent mutation).

## License

See repository root `LICENSE` for licensing information.


