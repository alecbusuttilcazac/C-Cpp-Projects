// Demonstration of table::Table API usage (generated with AI assistance)
#include "Table.hpp"
#include <iostream>

int main() {
	using namespace table;

	// 1. Create a table and insert some rows
	Table table1;
    Table table2;
    
	table1.insertRow({"id", "name", "score", "age_opt"});      // could be used as header later
	table1.insertRow({"1", "Alice", "87"});
	table1.insertRow({"2", "Bob", "91", "24"});
	table1.insertRow({"3", "Cara", "78"});
    
    // OR
    
    table2.setTable({
        {"id", "name", "score", "age_opt"},
        {"1", "Alice", "87"},
        {"2", "Bob", "91", "24"},
        {"3", "Cara", "78"}
    });

	// 2. Set header from first row
	table1.setHeader(0); // now row 0 contents stored as header
	// Remove the original header row from data if you want pure data rows
	table1.removeRow(0);

	// 3. Query table size (height) and demonstrate indexed access
	std::cout << "Rows in table: " << table1.getHeight() << "\n";

	// 4. Fetch a specific row using operator[] and print via [][] notation
	try {
		auto row2 = table1[1]; // RowProxy for second data row (Bob)
		std::cout << "Row 1: ";
		for (size_t i = 0; i < row2.size(); ++i) {
			if (i) std::cout << ',';
			std::cout << row2[i];
		}
		std::cout << "\n";
	} catch (const RowOutOfBoundsException& e) {
		std::cerr << "Failed to access row 1: " << e.what() << '\n';
	}

	// 5. Get a specific field (row, column)
	try {
		// Use [][] notation to access a specific field
		std::string bobScore = table1[1][2]; // row 1 (Bob), column 2 (score)
		std::cout << "Bob's score: " << bobScore << "\n";
	} catch (const table::RowOutOfBoundsException& e) {
		std::cerr << e.what() << '\n';
	} catch (const table::ColumnOutOfBoundsException& e) {
		std::cerr << e.what() << '\n';
	}

	// 6. Copy the entire table (deep copy)
	auto snapshot = table1.copy();
	std::cout << "Snapshot rows: " << snapshot.size() << "\n";

	// 7. Demonstrate failure on invalid row access
	try {
		// Demonstrate failure on invalid row access using operator[]
		table1[100]; // out of bounds
	} catch (const RowOutOfBoundsException& e) {
		std::cerr << "Caught expected RowOutOfBoundsException: " << e.what() << '\n';
	}

	// 8. Insert at a specific position
	table1.insertRow({"4", "Dylan", "82"},  1); // insert before current row index 1
	table1.insertRow({"4", "Dylan", "82"});
	std::cout << "After insertion, row order:\n";
	// Use index-based loops and [][] access to print the table content
	for (uint32 ri = 0; ri < table1.getHeight(); ++ri) {
		auto r = table1[ri]; // RowProxy for row ri
		std::cout << "  [";
		for (size_t ci = 0; ci < r.size(); ++ci) {
			if (ci) std::cout << ',';
			std::cout << r[ci];
		}
		std::cout << "]\n";
	}

	// 9. Remove a row
	table1.removeRow(2); // remove what is now Cara or Dylan depending on order
	std::cout << "Rows after removal: " << table1.getHeight() << "\n";

	// 10. Show header access
	try {
		auto header = table1.getHeader();
		std::cout << "Header columns: ";
		for (size_t i = 0; i < header.size(); ++i) {
			if (i) std::cout << ',';
			std::cout << header[i];
		}
		std::cout << "\n";
	} catch (const NoTableHeaderException& e) {
		std::cerr << e.what() << '\n';
	}

	// Program end
	return 0;
}