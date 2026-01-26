# CPP-Projects
A collection of C++ projects for learning and experimentation.

---
**Note on AI Assistance:**
All code in these projects was written almost entirely by me. AI (GitHub Copilot) was used only sparingly for guidance, tough bug fixes, and documentation — not for generating code in large batches.

## Included projects
-----------------

- `csv-library/` — lightweight C++17 CSV utilities (header-only). It provides:
	- Reader: header-aware reading, multi-line field support, custom delimiters.
	- Writer: RFC-style quoting/escaping and append/overwrite modes.
	- ReaderWriter: convenience class that opens the same file for both reading and writing.

	Quick build hint (from the `csv-library` directory):

	```bash
	g++ -std=c++17 CSVuser.cpp -o bin/CSVuser
	./bin/CSVuser
	```

	See `csv-library/README.md` for full details and usage examples.
	
- `reduciblearray-data-structure/` — robust C++17 dynamic array with activeness tracking. It provides:
	- `ReducibleArray<T>`: a custom container for explicit memory management, element activeness, and safe access.
	- Exception-based error handling and iterators for active/all elements.
	- Example usage and tests in `ReducibleArrayUser.cpp`.
	- C version is experimental and not complete.

	Quick build hint (from the `reduciblearray-data-structure` directory):

	```bash
	g++ -std=c++17 ReducibleArrayUser.cpp -o bin/ReducibleArrayUser
	./bin/ReducibleArrayUser
	```

	See `reduciblearray-data-structure/README.md` for more details.

- `matrix-library/` — minimal C++17 header-only matrix utility for 2D matrices (vectors of vectors). It provides:
	- `Matrix`: a simple 2D matrix class for basic operations (construction, element access, resizing).
	- Example usage and tests in `MatrixUser.cpp`.
	- Only supports 2D matrices; not a complete or production-ready library.

	Quick build hint (from the `matrix-library` directory):

	```bash
	g++ -std=c++17 MatrixUser.cpp -o bin/MatrixUser
	./bin/MatrixUser
	```

	See `matrix-library/README.md` for more details.




- `table-library/` — small header-only Table utility (C++17). It provides:
	- `table::Table`: a lightweight 2D table abstraction (rows of string fields) with checked accessors, row proxies for [][] access, and convenience methods (view/copy/rowRef/insertRow/removeRow).
	- Exception types for precise error handling (RowOutOfBoundsException, ColumnOutOfBoundsException, NoTableHeaderException).

	Quick build hint (from the `table-library` directory):

	```bash
	g++ -std=c++17 TableUser.cpp -I. -o bin/TableUser
	./bin/TableUser
	```

	See `table-library/` for examples and header documentation.