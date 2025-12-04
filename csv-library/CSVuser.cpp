#include "include/TypeDef.hpp"
#include "include/Error.hpp"
#include "include/Table.hpp"
#include "CSV.hpp"


// Demo program for csv::Reader / csv::Writer and table::Table
// - Shows basic read/write usage and how to access table cells using [] and [][] notation.
// - Uses explicit indexed iteration over table rows (table.rowRef(i)) so row[col] works reliably.

using namespace std;

int main() {    
    // Writer usage: write a few example rows to "example.csv".
    // Demonstrates writeRow(), flush(), delimiter management and getRowNumber().
    csv::Writer* w = new csv::Writer;
    w->setWarningCallback([](const std::string& msg) {
        std::cerr << "CSV Writer Warning: " << msg << std::endl;
    });
    w->open("example.csv", true);
    w->writeRow({"field1", "field2", "field3"});
    w->writeRow({"value1", "42", "3.14"});
    w->flush();
    w->writeRow({"value2", "100", "2.71"});
    w->writeRow({"value3", "7", "1.23"});
    w->flush();
    cout << "file is " << (w->isOpen() ? "open" : "closed") << std::endl;
    std::cout << "delimiter is " << w->getDelimiter() << std::endl;
    w->setDelimiter('+');
    std::cout << "delimiter is " << w->getDelimiter() << std::endl;
    std::cout << "current line is " << w->getRowNumber() << std::endl;
    w->close();
    delete w;

    // Reader usage: open the same file and demonstrate reading.
    // We open with a start offset to show setHeader() and row navigation examples.
    csv::Reader r(1);
    r.setWarningCallback([](const std::string& msg) {
        std::cerr << "CSV Reader Warning: " << msg << std::endl;
    });
    r.open("example.csv", 1);
    r.setHeader();
    r.setHeader(0); // Reset header to first line

    // Exception handling
    try {
        throw error::NonFatalException("hi");
    } catch (const error::NonFatalException& e) {
        std::cerr << "\nCSV error caught:\n\t" << e.what() << std::endl;
    }
    std::cout << "current line is " << r.getRowNumber() << std::endl;

    // Row navigation
    r.setRowNumber(1);
    r.skipLines(1);
    std::cout << "current line is " << r.getRowNumber() << std::endl;
    {
        auto dummy = r.readRow();
    }
    std::cout << "current line is " << r.getRowNumber() << std::endl;

    // Read single row
    auto row = r.readRow();
    std::cout << "the row you just read:\n    [";
    if (row) {
        bool firstElement = true;
        for (const auto& item : *row) {
            if (!firstElement) std::cout << ",";
            std::cout << item;
            firstElement = false;
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "current line is " << r.getRowNumber() << std::endl;

    // Read all rows into a table. Iterate by index and use rowRef(i)
    // so we can use row[col] (i.e. [][] notation) and avoid any iterator
    // overload issues with custom Table types.
    auto table = r.readAll();
    std::cout << "the rows you just read:\n";
    uint32 totalRows = table.getHeight();
    for (uint32 ri = 0; ri < totalRows; ++ri) {
        const auto& rowRef = table.rowRef(ri); // reference to internal row vector
        bool firstElement = true;
        std::cout << "    [";
        for (size_t ci = 0; ci < rowRef.size(); ++ci) {
            if (!firstElement) std::cout << ",";
            // Use [][]-style access: table[ri][ci] would also work via proxies
            std::cout << rowRef[ci];
            firstElement = false;
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "current line is " << r.getRowNumber() << std::endl;

    // Change row number and delimiter
    r.setRowNumber(3);
    row = r.readRow(',');
    std::cout << "the row you just read:\n    [";
    if (row) {
        bool firstElement = true;
        for (const auto& item : *row) {
            if (!firstElement) std::cout << ",";
            std::cout << item;
            firstElement = false;
        }
        std::cout << "]" << std::endl;
    } else {
        std::cout << "(no data)]" << std::endl;
    }

    // Demonstrate getColumn
    try {
        auto col = r.getColumn("field2");
        std::cout << "Column 'field2': [";
        bool firstElement = true;
        for (const auto& item : col) {
            if (!firstElement) std::cout << ",";
            std::cout << item;
            firstElement = false;
        }
        std::cout << "]" << std::endl;
    } catch (const csv::SchemaMismatchException& e) {
        std::cerr << "Column 'field2' not found: " << e.what() << std::endl;
    }

    // Demonstrate getFieldByType
    r.setRowNumber(2);
    try {
        std::string field = r.getFieldByType(r.getRowNumber(), "field2");
        std::cout << "Field 'field2' as string: " << field << std::endl;
    } catch (const csv::SchemaMismatchException& e) {
        std::cerr << "Field 'field2' not found: " << e.what() << std::endl;
    } catch (const csv::ShortRowException& e) {
        std::cerr << "Row too short: " << e.what() << std::endl;
    }

    // Demonstrate error handling for file not found
    try{
        csv::Reader r2;
        r2.open("nonexistent.csv");
    }catch (const error::FatalException& e) {
        std::cerr << e.what() << std::endl;
    }catch (const error::NonFatalException& e) {
        std::cerr << e.what() << std::endl;
    }

    // Demonstrate warning callback for malformed row
    r.setWarningCallback([](const std::string& msg) {
        std::cerr << "Custom warning: " << msg << std::endl;
    });
    r.setRowNumber(0);
    auto malformed = r.readRow(); // If any malformed row, warning will be printed

    // Demonstrate closing and reopening
    r.close();
    r.open("example.csv");
    std::cout << "Reopened file, current line: " << r.getRowNumber() << std::endl;

    // Demonstrate setDelimiter and reading with new delimiter
    r.setDelimiter('+');
    r.setRowNumber(4);
    auto plusRow = r.readRow();
    std::cout << "Row with '+' delimiter: [";
    if (plusRow) {
        bool firstElement = true;
        for (const auto& item : *plusRow) {
            if (!firstElement) std::cout << ",";
            std::cout << item;
            firstElement = false;
        }
        std::cout << "]" << std::endl;
    } else {
        std::cout << "(no data)]" << std::endl;
    }

    // Demonstrate closing
    r.close();
    std::cout << "File closed." << std::endl;

    return 0;
}