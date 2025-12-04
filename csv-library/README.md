# cpp-csv-library

Lightweight C++17 CSV utilities: Reader, Writer and ReaderWriter classes plus helpful exceptions and utilities for robust CSV handling.

## Overview

This small header-only library (single header `CSV.h`) provides utilities to read and write CSV files with support for:

- RFC-4180 style quoting (double quotes escaping by doubling: `""`).
- Multi-line fields (fields containing embedded newlines inside quotes).
- Custom delimiters (default is comma `,`).
- Header-aware access (set header row and access fields by column name).
- Combined read/write access via `ReaderWriter` for working with the same file.
- Fine-grained exceptions and optional warning callbacks.

The code requires C++17 or newer.

## Files

- `CSV.h` — main header; contains `CSV::Reader`, `CSV::Writer`, `CSV::ReaderWriter`, exceptions and `countLines()` utility.
- `CSVuser.cpp` — example/usage file demonstrating the API.
- `headers/CppTypeDef.h` and `headers/CTypeDef.h` — small typedefs and colour macros used by the project.

## Key classes and API

All classes live in namespace `CSV`.

1) CSV::Reader

- Constructor: `Reader(char delimiter = ',', uint32 startRow = 0)`
- open(const std::string& path, uint32 startLine = 0) — open a file for reading
- close() — close file
- isOpen(), isEOF()
- setHeader(), setHeader(uint32 headerRow), isHeaderSet()
- readRow(), readRow(char delim) — reads next row; returns `std::optional<std::vector<std::string>>`
- readAll(), readAll(char delim) — reads all rows; returns `std::optional<std::vector<std::vector<std::string>>>`
- getFieldByType(uint32 rowNumber, const std::string& columnName)
- getFieldByType(const std::vector<std::string>& row, const std::string& columnName) const
- getColumn(const std::string& columnName)
- setDelimiter(char), getDelimiter()
- setRowNumber(uint32 targetRow), getRowNumber(), skipLines(uint32 count)
- getNumRows() — returns counted number of rows (uses `countLines()` internally on open)
- setWarningCallback(std::function<void(const std::string&)>) — set a callback to receive warnings (e.g., EOF reached early)

2) CSV::Writer

- Constructor: `Writer(char delimiter = ',')`
- open(const std::string& path, bool overwrite = false) — open for writing (append or overwrite)
- close(), isOpen(), flush()
- writeRow(const std::vector<std::string>& row), writeRow(..., char delim)
- writeAll(const std::vector<std::vector<std::string>>& table), writeAll(..., char delim)
- getRowNumber(), getNumRows() — track rows written
- setDelimiter(char), getDelimiter()
- setWarningCallback(...) — receive non-fatal warnings

Notes: `Writer::parseString(...)` quotes and escapes fields per RFC (double-quotes doubled). `writeRow` appends a trailing `\n` (no constant flush).

3) CSV::ReaderWriter

- Combined reader and writer for the same file. Mirrors main methods from `Reader` and `Writer` and keeps internal line counters consistent.

4) Utility

- `uint32 CSV::countLines(const std::string& filePath)` — returns the number of lines in the file (throws if file cannot be opened).

## Exceptions

Two base categories are provided:

- `FatalException` (unrecoverable) — e.g., `ParseException`, `WriterClosedException`.
- `NonFatalException` (recoverable/warning-like) — e.g., `ReaderClosedException`, `NoHeaderException`, `ShortRowException`, `InvalidLineException`, `FileOpenFailureException`, `readRowException`, `WriteLineException`.

Use try/catch on `CSV::FatalException` / `CSV::NonFatalException` (or the derived types) to handle errors.

## Behavior and edge cases

- Quoting and escaping: fields that contain the delimiter, a newline, or a double-quote are written quoted; double-quotes inside fields are escaped by doubling (""), matching common CSV conventions.
- Multi-line fields are supported: `Reader` tracks quote state and only finishes a row when a newline is seen outside quotes.
- If a row has fewer fields than the set header, a `ShortRowException` is thrown.
- Attempting header-related operations before setting the header will throw `NoHeaderException`.
- `readRow()` returns `std::nullopt` at EOF (and warning callback may be triggered).
- `setRowNumber()` may rewind the file and re-read lines as necessary; it's a convenience navigation method but not optimized for giant files.

## Usage examples

Writer example (from `CSVuser.cpp`):

```cpp
CSV::Writer w;
w.setWarningCallback([](const std::string& msg){ std::cerr << "CSV Writer Warning: " << msg << std::endl; });
w.open("example.csv", true); // append mode
w.writeRow({"field1", "field2", "field3"});
w.writeRow({"value1", "42", "3.14"});
w.flush();
w.close();
```

Reader example (from `CSVuser.cpp`):

```cpp
CSV::Reader r;
r.open("example.csv", 0);
r.setHeader(); // set header from current row
auto row = r.readRow();
if (row) {
	for (auto &v : *row) std::cout << v << ", ";
}
r.close();
```

Getting a column by name and a single field by row/column name:

```cpp
auto col = r.getColumn("field2");
std::string s = r.getFieldByType(2, "field2");
```

ReaderWriter (combined) example:

```cpp
CSV::ReaderWriter rw;
rw.open("example.csv", 0);
rw.setHeader();
auto all = rw.readAll();
rw.writeRow({"a","b","c"});
rw.flush();
rw.close();
```

## Build and run (quick)

From the `cpp-csv-library` directory you can compile the example with g++ (C++17):

```bash
g++ -std=c++17 CSVuser.cpp -o bin/CSVuser
./bin/CSVuser
```

This repository also contains VS Code tasks that create `bin/` and compile the example automatically.

## Limitations and TODOs

- The library is header-only but not a polished distribution package.
- `setRowNumber()` rewinds and re-reads lines; it is not optimized for very large files (no index or random access table). For large files, consider streaming-only operations.
- There are TODOs in `CSV.h` mentioning potential additions such as `find()` and more helper functions.
- No unit tests currently included in the repo; adding tests would improve reliability.

## Contributing

Feel free to open issues or PRs. Small improvements that help robustness (additional tests, better error messages, more efficient seeking/indexing) are welcome.

## License

See repository root `LICENSE` for licensing information.

---
Generated from `CSV.h` and `CSVuser.cpp` examples. If anything here is incorrect or missing, open an issue or send a patch with suggested clarifications.

