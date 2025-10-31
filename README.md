# C-Cpp-Projects
A collection of C and C++ projects for learning and experimentation.

Includes a small header-only CSV utility in `cpp-csv-library/` providing Reader, Writer and ReaderWriter helpers (C++17) for robust CSV read/write with RFC-style quoting and header-aware access.

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
