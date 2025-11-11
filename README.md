# C-Cpp-Projects
A collection of C and C++ projects for learning and experimentation.

## Included projects
-----------------

- `cpp-csv-library/` — lightweight C++17 CSV utilities (header-only). It provides:
	- Reader: header-aware reading, multi-line field support, custom delimiters.
	- Writer: RFC-style quoting/escaping and append/overwrite modes.
	- ReaderWriter: convenience class that opens the same file for both reading and writing.

	Quick build hint (from the `cpp-csv-library` directory):

	```bash
	g++ -std=c++17 CSVuser.cpp -o bin/CSVuser
	./bin/CSVuser
	```

	See `cpp-csv-library/README.md` for full details and usage examples.

- `cpp-table-library/` — small header-only Table utility (C++17). It provides:
	- `table::Table`: a lightweight 2D table abstraction (rows of string fields) with checked accessors, row proxies for [][] access, and convenience methods (view/copy/rowRef/insertRow/removeRow).
	- Exception types for precise error handling (RowOutOfBoundsException, ColumnOutOfBoundsException, NoTableHeaderException).

	Quick build hint (from the `cpp-table-library` directory):

	```bash
	g++ -std=c++17 TableUser.cpp -I. -o bin/TableUser
	./bin/TableUser
	```

	See `cpp-table-library/` for examples and header documentation.
