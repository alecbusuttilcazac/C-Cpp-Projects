// CSV.h - CSV file utilities for C++17 and later
// Requires C++17 or newer. Not compatible with C or earlier C++ standards.
#ifndef __cplusplus // Ensure C++ compilation
#error This header requires a C++ compiler.
#elif __cplusplus < 201703L // Ensure C++17 or newer
#error This header requires at least C++17.
#endif

#pragma once

// Standard library includes
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <optional>

#include "include/TypeDef.hpp" // Project-specific typedefs
#include "include/Error.hpp"  // Project-specific error handling
#include "include/Table.hpp"  // Table class

// TODO:
// - Add find() function
// - Implement more useful functions
// - Bug fixes
// - Update test/example .cpp files

namespace csv{   
     
// Free utility functions (not part of any class):
inline uint32 countLines(const std::string& filePath); // Count lines in a file

// Specific fatal exceptions
class WriterClosedException : public error::FatalException{
public:
    explicit WriterClosedException()
    :   FatalException("Writer functions inaccessible as writer is closed.")
    {}
};

class ParseException : public error::FatalException{
public:
    explicit ParseException(uint32 rowNumber, const std::string& filePath)
    :   FatalException("Parsing error at row number "+std::to_string(rowNumber)+".\n\t->\tFile: \""+filePath+"\".")
    {}
    explicit ParseException(uint32 rowNumber, const std::string& filePath, const std::string& errorMessage)
    :   FatalException("Parsing error at row number "+std::to_string(rowNumber)+".\n\t->\tFile: \""+filePath+"\"\n\t->\tAdditional Information: "+errorMessage)
    {}
};

class SchemaMismatchException : public error::FatalException{
public:
    explicit SchemaMismatchException(const std::string& filePath, const std::string& attemptedName)
    :   FatalException("\""+attemptedName+"\" is not a member of set header.\n\t->\tFile: \""+filePath+"\"")
    {}
    explicit SchemaMismatchException(const std::string& filePath, const std::string& attemptedName, const std::vector<std::string>& header, char delimiter = ',')
    :   FatalException("\""+attemptedName+"\" is not a member of set header.\n\t->\tHeader: "+toString(header, delimiter)+"\n\t->\tFile: \""+filePath+"\"")
    {}
private:
    static std::string toString(const std::vector<std::string>& row, char delimiter){
        bool firstElement = true;
        std::string returnString;
        returnString = "[";
        
        for(const auto& field : row){
            if(firstElement){
                returnString += field; 
                firstElement = false;
            }
            else {
                returnString += delimiter;
                returnString += field;
            }
        }
        return returnString += "]";
    }
};

// Specific non-fatal exceptions

class ReaderClosedException : public error::NonFatalException{
public:
    explicit ReaderClosedException()
    :   NonFatalException("Reader functions inaccessible as writer is closed.")
    {}
};
class ReaderWriterClosedException : public error::NonFatalException{
public:
    explicit ReaderWriterClosedException()
    :   NonFatalException("ReaderWriter functions inaccessible as ReaderWriter is closed.")
    {}
};

class NoFileHeaderException : public error::NonFatalException{
public:
    explicit NoFileHeaderException()
    :   NonFatalException("Header not set before calling header-related function.")
    {}
    explicit NoFileHeaderException(const std::string& filePath)
    :   NonFatalException("Header for "+filePath+" not set before calling header-related function.")
    {}
    explicit NoFileHeaderException(const std::string& filePath, const std::vector<std::string>& possibleHeader, char delimiter = ',')
    :   NonFatalException("Header not set before calling header-related function.\n\t->\tLikely header at row number 0: "+toString(possibleHeader, delimiter)+"\n\t->\tFile: \""+filePath+"\"")
    {}
private:
    static std::string toString(const std::vector<std::string>& row, char delimiter){
        bool firstElement = true;
        std::string returnString;
        returnString = "[";
        
        for(const auto& field : row){
            if(firstElement){
                returnString += field; 
                firstElement = false;
            }
            else {
                returnString += delimiter;
                returnString += field;
            }
        }
        return returnString += "]";
    }
};

class ShortRowException : public error::NonFatalException{
public:
    explicit ShortRowException(const std::string& filePath, const std::vector<std::string>& header, size_t attemptedSize, char delimiter = ',')
    :   NonFatalException("Row of size "+std::to_string(attemptedSize)+" too short for size "+std::to_string(header.size())+" in set header.\n\t->\tHeader: "+toString(header, delimiter)+"\n\t->\tFile: \""+filePath+"\"")
    {}
private:
    static std::string toString(const std::vector<std::string>& row, char delimiter){
        bool firstElement = true;
        std::string returnString;
        returnString = "[";
        
        for(const auto& field : row){
            if(firstElement){
                returnString += field; 
                firstElement = false;
            }
            else {
                returnString += delimiter;
                returnString += field;
            }
        }
        return returnString += "]";
    }
};

// InvalidLineException: usually when trying to read a row past EOF
class InvalidLineException : public error::NonFatalException{
public:
    explicit InvalidLineException(uint32 rowNumber)
    :   NonFatalException("Row number "+std::to_string(rowNumber)+" exceeds file length.")
    {}
    explicit InvalidLineException(uint32 rowNumber, uint32 numLinesInFile)
    :   NonFatalException("Row number "+std::to_string(rowNumber)+" exceeds file length of "+std::to_string(numLinesInFile)+".")
    {}
    explicit InvalidLineException(uint32 rowNumber, const std::string& filePath)
    :   NonFatalException("Row number "+std::to_string(rowNumber)+" exceeds file length.\n\t->\tFile: \""+filePath+"\"")
    {}
    explicit InvalidLineException(uint32 rowNumber, uint32 numLinesInFile, const std::string& filePath)
    :   NonFatalException("Row number "+std::to_string(rowNumber)+" exceeds file length of "+std::to_string(numLinesInFile)+".\n\t->\tFile: \""+filePath+"\"")
    {}
};

// FileOpenFailureException: when file fails to open for reading/writing
class FileOpenFailureException : public error::NonFatalException{
public:
    explicit FileOpenFailureException(const std::string& filePath)
    :   NonFatalException("File "+filePath+" failed to open.")
    {}
};

// readRowException: when rowNumber fails to be read
class readRowException : public error::NonFatalException{
public:
    explicit readRowException(uint32 rowNumber)
    :   NonFatalException("Failed to read row " +std::to_string(rowNumber)+".")
    {}
    explicit readRowException(const std::string& filePath)
    :   NonFatalException("Failed to read row in file "+filePath)
    {}
    explicit readRowException(uint32 rowNumber, const std::string& filePath)
    :   NonFatalException("Failed to read row "+std::to_string(rowNumber)+".\n\t->\tFile: \""+filePath+"\"")
    {}
};

// WriteLineException: when rowNumber fails to be written
class WriteLineException : public error::NonFatalException{
public:
    explicit WriteLineException(uint32 rowNumber)
    :   NonFatalException("Failed to write row number " +std::to_string(rowNumber))
    {}
    explicit WriteLineException(uint32 rowNumber, const std::string& filePath)
    :   NonFatalException("Failed to write row number "+std::to_string(rowNumber)+".\n\t->\tFile: \""+filePath+"\"")
    {}
};


// CSV Reader: strictly for reading CSV files
class Reader{
private:
    std::ifstream file; // Input file stream
    std::string path;   // Path to CSV file
    std::vector<std::string> header; // CSV header row
    char delimiter;     // Delimiter character (default ',')
    uint32 rowNumber;   // Current row number (0-based)
    uint32 numRows;     // Total number of rows in file
    std::function<void(const std::string&)> warningCallback; // Warning callback
    
    // Internal parsing helpers
    void parseString(const std::string& lineStr, std::vector<std::string>& fields) const;
    void parseString(const std::string& lineStr, std::vector<std::string>& fields, char delim) const;
    void setNumLines(uint32 numRows); // Set total number of lines
    friend class ReaderWriter; // Allow ReaderWriter access
    
public:
    Reader(char delimiter = ',', uint32 startRow = 0)
    :   delimiter(delimiter),
        rowNumber(startRow),
        numRows(0)
    {}
    ~Reader() = default;
    
    void setWarningCallback(const std::function<void(const std::string&)>& cb) {
        warningCallback = cb;
    }
    
    // File operations
    void open(const std::string& filePath, uint32 rowNumber = 0); // Open file
    bool isOpen() const noexcept; // Is file open?
    bool isEOF() const; // End of file reached?
    void close() noexcept; // Close file
    
    // Header operations
    void setHeader(); // Set header from current row
    void setHeader(uint32 headerRow); // Set header from specific row
    bool isHeaderSet() const; // Is header set?
    
    // Row reading
    std::optional<std::vector<std::string>> readRow(); // Read next row
    std::optional<std::vector<std::string>> readRow(char delim); // Read next row with custom delimiter
    // Read all rows
    table::Table readAll();
    table::Table readAll(char delim);
    
    // Field and column access
    std::string getFieldByType(uint32 rowNumber, const std::string& columnName); // Get field by row and column name
    std::string getFieldByType(const std::vector<std::string>& row, const std::string& columnName) const; // Get field by column name from row
    std::vector<std::string> getColumn(const std::string& columnName); // Get entire column by name
    
    // Delimiter access
    char getDelimiter() const;
    void setDelimiter(char delim);
    
    // Row navigation
    uint32 getRowNumber() const; // Get current row number
    void setRowNumber(uint32 targetRow); // Jump to specific row
    void skipLines(uint32 count); // Skip lines
    uint32 getNumRows() const; // Get total number of rows
};

// CSV Writer: strictly for writing CSV files
class Writer{
private:
    std::ofstream file; // Output file stream
    std::string path;   // Path to CSV file
    char delimiter;     // Delimiter character (default ',')
    uint32 rowNumber;   // Current row number (0-based)
    uint32 numRows;     // Total number of rows written
    std::function<void(const std::string&)> warningCallback; // Warning callback
    
    // Internal parsing helpers
    std::string parseString(const std::string& field) const; // Quote/escape field
    std::string parseString(const std::string& field, char delim) const; // Quote/escape field with custom delimiter
    void setNumLines(uint32 numRows); // Set total number of lines

public:
    Writer(char delimiter = ',')
    :   delimiter(delimiter),
        rowNumber(0),
        numRows(0)
    {}
    ~Writer() = default;
    
    // Set warning callback
    void setWarningCallback(const std::function<void(const std::string&)>& cb) {
        warningCallback = cb;
    }
    
    // File operations
    void open(const std::string& filePath, bool overwrite = false); // Open file for writing
    bool isOpen() const noexcept; // Is file open?
    void close() noexcept; // Close file
    
    // Row writing
    void writeRow(const std::vector<std::string>& row); // Write a row
    void writeRow(const std::vector<std::string>& row, char delim); // Write a row with custom delimiter
    void writeAll(const table::Table& t); // Write all rows
    void writeAll(const table::Table& t, char delim); // Write all rows with custom delimiter
    void flush(); // Flush output
    
    // Delimiter access
    char getDelimiter() const;
    void setDelimiter(char delim);
    
    // Row navigation
    uint32 getRowNumber() const; // Get current row number
    uint32 getNumRows() const; // Get total number of rows written
};

// CSV ReaderWriter: combined reading and writing on the same file
class ReaderWriter{
private:
    Reader reader; // Internal Reader instance
    Writer writer; // Internal Writer instance
    std::function<void(const std::string&)> warningCallback; // Warning callback
    
public:
    ReaderWriter(char delimiter = ',', uint32 startLine = 0)
    :   reader(delimiter, startLine),
        writer(delimiter)
    {}
    ~ReaderWriter() = default;
    
    // Set warning callback for both reader and writer
    void setWarningCallback(const std::function<void(const std::string&)>& cb) {
        warningCallback = cb;
        reader.setWarningCallback(cb);
        writer.setWarningCallback(cb);
    }
    
    // File operations
    void open(const std::string& filePath, uint32 rowNumber = 0); // Open file for reading/writing
    bool isOpen() const noexcept; // Is file open?
    bool isEOF() const; // End of file reached?
    void close() noexcept; // Close file
    
    // Header operations
    void setHeader(); // Set header from current row
    void setHeader(uint32 headerRow); // Set header from specific row
    bool isHeaderSet() const; // Is header set?
    
    // Field and column access
    std::string getFieldByType(const std::vector<std::string>& row, const std::string& columnName) const; // Get field by column name from row
    std::string getFieldByType(uint32 rowNumber, const std::string& columnName); // Get field by row and column name
    
    // Row reading
    std::optional<std::vector<std::string>> readRow(); // Read next row
    std::optional<std::vector<std::string>> readRow(char delim); // Read next row with custom delimiter
    // Read all rows
    table::Table readAll();
    table::Table readAll(char delim);
    
    // Row writing
    void writeRow(const std::vector<std::string>& row); // Write a row
    void writeRow(const std::vector<std::string>& row, char delim); // Write a row with custom delimiter
    void writeAll(const table::Table& t); // Write all rows
    void writeAll(const table::Table& t, char delim); // Write all rows with custom delimiter
    void flush(); // Flush output
    
    // Delimiter access
    char getDelimiter() const;
    void setDelimiter(char delim);
    
    // Row navigation
    uint32 getReaderLine() const; // Get current reader line
    uint32 getNumRows() const; // Get total number of rows
    void setReaderLine(uint32 targetRow); // Jump to specific reader line
    void skipLines(uint32 count); // Skip lines
    uint32 getWriterLine() const; // Get current writer line
};


//  === READER METHODS ===

inline bool Reader::isOpen() const noexcept { return file.is_open(); }

inline bool Reader::isHeaderSet() const{
    if(!isOpen()) throw ReaderClosedException();
    return !header.empty();
}

inline char Reader::getDelimiter() const{
    if(!isOpen()) throw ReaderClosedException();
    std::cout << COLOUR_BLACK;
    return delimiter;
}

inline void Reader::setDelimiter(char delim){
    if(!isOpen()) throw ReaderClosedException();
    delimiter = delim; 
}

inline uint32 Reader::getNumRows() const{
    if(!isOpen()) throw ReaderClosedException();
    return numRows;
}
inline void Reader::setNumLines(uint32 numRows){
    if(!isOpen()) throw ReaderClosedException();
    this->numRows = numRows; 
}

inline void Reader::setHeader(){
    if(!isOpen()) throw ReaderClosedException();
    
    auto row = readRow();
    if(!row) throw InvalidLineException(rowNumber, path);
    header = std::move(*row);
}

inline uint32 Reader::getRowNumber() const {
    if(!isOpen()) throw ReaderClosedException();
    return rowNumber;
}

inline void Reader::setRowNumber(uint32 targetRow){
    if(!isOpen()) throw ReaderClosedException();
    
    if(rowNumber == targetRow) return;
    
    if(rowNumber > targetRow){
        file.seekg(0, std::ios::beg);
        rowNumber = 0;  // reset after rewind
        
        for (uint32 i = 0; i < targetRow; ++i) {
            auto maybeRow = readRow();
            if (!maybeRow) throw InvalidLineException(targetRow, numRows, path);
            // row number is already incremented by readRow()
        }
    }else{ // if row number < targetRow
        for (uint32 i = rowNumber; i < targetRow; ++i) {
            auto maybeRow = readRow();
            if (!maybeRow) {
                if(file.eof()) return;
                throw InvalidLineException(targetRow, numRows, path);
            }
            // row number is already incremented by readRow()
        }
    }
}

inline void Reader::skipLines(uint32 numRows){
    if(!isOpen()) throw ReaderClosedException();
    
    for (uint32 i = 0; i < numRows; ++i) {
        auto row = readRow();
        if (!row) {
            if(file.eof()){
                if (warningCallback) warningCallback("Reached EOF before skipping all requested lines.");
                return;
            }
            throw InvalidLineException(numRows, path);
        }
    }
    // No return needed; function is void
}

inline void Reader::open(const std::string& filePath, uint32 startLine){
    path = filePath;
    if (file.is_open()) file.close(); // close open file
    
    file.open(filePath, std::ios::in);
    if(!file.is_open()) throw FileOpenFailureException(filePath);
    numRows = countLines(filePath);
    
    if (startLine > 0){
        skipLines(startLine);
    }else rowNumber = 0;
}

inline void Reader::setHeader(uint32 headerRow){
    if(!isOpen()) throw ReaderClosedException();
    
    uint32 originalLine = rowNumber;
    setRowNumber(headerRow);
    
    auto row = readRow();
    if(!row) throw InvalidLineException(headerRow, path);
    header = std::move(*row);
    setRowNumber(originalLine);
}

// Add this implementation before Reader::readRow() (around row number 272):
inline void Reader::parseString(const std::string& lineStr, std::vector<std::string>& fields) const {
    fields.clear();
    fields.reserve(10);
    
    std::string field;
    field.reserve(64);
    bool inQuotes = false;
    bool lastWasQuote = false;
    
    for(size_t i = 0; i < lineStr.size(); ++i){
        char c = lineStr[i];
        
        if(inQuotes){
            if(c == '"'){
                if(lastWasQuote){
                    field += '"';
                    lastWasQuote = false;
                }else lastWasQuote = true;
            }else if(lastWasQuote){
                inQuotes = false;
                lastWasQuote = false;
                
                if(c == delimiter){
                    fields.push_back(std::move(field));
                    field.clear();
                    field.reserve(64);
                }else field += c;
            }else field += c;
        }else{
            if(c == '"'){
                inQuotes = true;
            }else if(c == delimiter){
                fields.push_back(std::move(field));
                field.clear();
                field.reserve(64);
            }else field += c;
        }
    }
    
    fields.push_back(std::move(field));
    if(inQuotes && !lastWasQuote) throw ParseException(rowNumber, path);
    
    // Trim trailing newlines
    for(auto& f : fields) {
        while(!f.empty() && (f.back() == '\n' || f.back() == '\r')) f.pop_back();
    }
}

inline void Reader::parseString(const std::string& lineStr, std::vector<std::string>& fields, char delim) const {
    fields.clear();
    fields.reserve(10);

    std::string field;
    field.reserve(64);
    bool inQuotes = false;
    bool lastWasQuote = false;

    for(size_t i = 0; i < lineStr.size(); ++i){
        char c = lineStr[i];

        if(inQuotes){
            if(c == '"'){
                if(lastWasQuote){
                    field += '"';
                    lastWasQuote = false;
                }else lastWasQuote = true;
            }else if(lastWasQuote){
                inQuotes = false;
                lastWasQuote = false;

                if(c == delim){
                    fields.push_back(std::move(field));
                    field.clear();
                    field.reserve(64);
                }else field += c;
            }else field += c;
        }else{
            if(c == '"'){
                inQuotes = true;
            }else if(c == delim){
                fields.push_back(std::move(field));
                field.clear();
                field.reserve(64);
            }else field += c;
        }
    }

    fields.push_back(std::move(field));
    if(inQuotes && !lastWasQuote) throw ParseException(rowNumber, path);

    // Trim trailing newlines
    for(auto& f : fields) {
        while(!f.empty() && (f.back() == '\n' || f.back() == '\r')) f.pop_back();
    }
}

inline std::vector<std::string> Reader::getColumn(const std::string& columnName){
    if(!isOpen()) throw ReaderClosedException();
    if(!isHeaderSet()) throw error::FatalException("Header not set before calling getFieldByType()");
    
    std::vector<std::string> columnVector;
    uint32 originalLine = rowNumber;
    setRowNumber(0);
    
    while(auto row = readRow()) columnVector.push_back(getFieldByType(*row, columnName));
    
    setRowNumber(originalLine);
    return columnVector;
}

inline std::string Reader::getFieldByType(const std::vector<std::string>& row, const std::string& columnName) const {
    if(!isOpen()) throw ReaderClosedException();
    if(!isHeaderSet()) throw NoFileHeaderException(path);

    for(size_t i = 0; i < header.size(); i++){
        if(header[i] == columnName){
            if(i >= row.size()) throw ShortRowException(path, header, row.size(), delimiter);
            return row[i];
        }
    }
    throw SchemaMismatchException(path, columnName, header, delimiter);
}

inline std::string Reader::getFieldByType(uint32 rowNumber, const std::string& columnName) {
    if(!isOpen()) throw ReaderClosedException();
    if(!isHeaderSet()) throw NoFileHeaderException(path);

    uint32 originalLine = rowNumber;
    setRowNumber(rowNumber);
    auto row = readRow();
    setRowNumber(originalLine); // Always restore position
    if (!row) throw InvalidLineException(rowNumber, path);

    const std::vector<std::string>& row2 = *row;
    for(size_t i = 0; i < header.size(); i++){
        if(header[i] == columnName){
            if(i >= row2.size()) throw ShortRowException(path, header, row2.size(), delimiter);
            return row2[i];
        }
    }
    throw SchemaMismatchException(path, columnName, header, delimiter);
}

inline std::optional<std::vector<std::string>> Reader::readRow(){        
    if(!isOpen()) throw ReaderClosedException();
    
    std::vector<std::string> RETURNvector;
    std::string lineStr;
    bool inQuotes = false;
    char c;
    
    // Read characters until we have a complete CSV row
    while(file.get(c)){
        lineStr += c;
        
        // Track quote state to detect multi-row fields
        if(c == '"'){
            // Check if it's an escaped quote
            if(file.peek() == '"') lineStr += file.get();  // consume the second quote
            else inQuotes = !inQuotes;  // toggle quote state
        }
        
        // If we hit a newline outside quotes, we have a complete row
        if(c == '\n' && !inQuotes){
            rowNumber++;
            parseString(lineStr, RETURNvector, delimiter);
            
            if (!header.empty() && RETURNvector.size() < header.size()) throw ShortRowException(path, header, RETURNvector.size(), delimiter);
            return RETURNvector;
        }
    }
    
    // Loop exited - check why
    if(file.eof()){
        // End of file - process any remaining content (last row without newline)
        if(!lineStr.empty()){
            rowNumber++;
            parseString(lineStr, RETURNvector, delimiter);
            
            if(!header.empty() && RETURNvector.size() < header.size()) throw ShortRowException(path, header, RETURNvector.size(), delimiter);
            return RETURNvector;
        }
        if (warningCallback) warningCallback("Reached EOF while reading rowNumber.");
        return std::nullopt; // null vector
    }
    
    throw readRowException(rowNumber, path);
}

inline std::optional<std::vector<std::string>> Reader::readRow(char delim){        
    if(!isOpen()) throw ReaderClosedException();
    
    std::vector<std::string> RETURNvector;
    std::string lineStr;
    bool inQuotes = false;
    char c;
    
    // Read characters until we have a complete CSV row
    while(file.get(c)){
        lineStr += c;
        
        // Track quote state to detect multi-row fields
        if(c == '"'){
            // Check if it's an escaped quote
            if(file.peek() == '"') lineStr += file.get();  // consume the second quote
            else inQuotes = !inQuotes;  // toggle quote state
        }
        
        // If we hit a newline outside quotes, we have a complete row
        if(c == '\n' && !inQuotes){
            rowNumber++;
            parseString(lineStr, RETURNvector, delim);
            
            if (!header.empty() && RETURNvector.size() < header.size()) throw ShortRowException(path, header, RETURNvector.size(), delim);
            return RETURNvector;
        }
    }
    
    // Loop exited - check why
    if(file.eof()){
        // End of file - process any remaining content (last rowNumber without newline)
        if(!lineStr.empty()){
            rowNumber++;
            parseString(lineStr, RETURNvector, delim);
            
            if(!header.empty() && RETURNvector.size() < header.size()) throw ShortRowException(path, header, RETURNvector.size(), delim);
            return RETURNvector;
        }
        if (warningCallback) warningCallback("Reached EOF while reading rowNumber.");
        return std::nullopt; // null vector
    }
    
    throw readRowException(rowNumber, path);
}

inline table::Table Reader::readAll(){
    if(!isOpen()) throw ReaderClosedException();
    uint32 originalRow = rowNumber;
    
    file.seekg(0, std::ios::beg); // rewind to start
    file.clear();
    
    rowNumber = 0;
    table::Table t;
    
    while (true) {
        auto maybeRow = readRow();
        if (!maybeRow) break;
        t.insertRow(std::move(*maybeRow));
    }
    
    // Clear EOF flag before restoring position
    file.clear();
    
    // Only restore if within valid range
    if(originalRow < t.getHeight()) setRowNumber(originalRow);
    else rowNumber = t.getHeight(); // Original position was past the data we read, stay at EOF
    
    return table::Table(std::move(t));
}

inline table::Table Reader::readAll(char delim){
    if(!isOpen()) throw ReaderClosedException();
    uint32 originalRow = rowNumber;
    
    file.seekg(0, std::ios::beg); // rewind to start
    file.clear();
    
    rowNumber = 0;
    table::Table t;
    
    while (true) {
        auto maybeRow = readRow(delim);
        if (!maybeRow) break;
        t.insertRow(std::move(*maybeRow));
    }
    
    // Clear EOF flag before restoring position
    file.clear();
    
    // Only restore if within valid range
    if(originalRow < t.getHeight()) setRowNumber(originalRow);
    else rowNumber = t.getHeight(); // Original position was past the data we read, stay at EOF
    
    return table::Table(std::move(t));
}

inline bool Reader::isEOF() const{
    if(!isOpen()) throw ReaderClosedException();
    return file.eof();                 // check the underlying stream
}

inline void Reader::close() noexcept {
    if (file.is_open()) file.close();
    
    header.clear();    // clear header vector
    rowNumber = 0;          // reset rowNumber counter
    path.clear();      // clear stored file path
}

//  === WRITER METHODS ===

inline bool Writer::isOpen() const noexcept { return file.is_open(); }

inline void Writer::flush() { 
    if(!isOpen()) throw WriterClosedException();
    file.flush(); 
}

inline char Writer::getDelimiter() const { 
    if(!isOpen()) throw WriterClosedException();
    return delimiter; 
}

inline void Writer::setDelimiter(char delim) { 
    if(!isOpen()) throw WriterClosedException();
    delimiter = delim; 
}

inline void Writer::setNumLines(uint32 numRows) { 
    if(!isOpen()) throw WriterClosedException();
    this->numRows = numRows; 
}

inline uint32 Writer::getNumRows() const {
    if(!isOpen()) throw WriterClosedException();
    return numRows;
}

inline uint32 Writer::getRowNumber() const{
if(!file.is_open()) throw error::FatalException("Writer is closed.");
    return rowNumber;
}

inline void Writer::open(const std::string& filePath, bool overwrite){
    
    // bool append: true = preserve contents, false = overwrite
    // bool appendAtEnd: if appending, append at beginning or end?
    
    path = filePath;
    if(file.is_open()) file.close(); // close open file
    
    if(!overwrite){
        file.open(filePath, std::ios::out | std::ios::app); // do not overwrite file
        rowNumber = countLines(filePath); // Count existing lines to append at end of file
        numRows = rowNumber;
    }else{
        file.open(filePath, std::ios::out | std::ios::trunc); // overwrite file and write
        rowNumber = 0;
        numRows = 0;
    }
    
    if(!file.is_open()) throw FileOpenFailureException(filePath);
}

// Add this implementation before Writer::writeRow() (around rowNumber 348):
inline std::string Writer::parseString(const std::string& field) const {
    // Check if field needs quoting (contains delimiter, newline, or quote)
    bool needsQuotes = (field.find(delimiter) != std::string::npos 
                        || field.find('\n') != std::string::npos 
                        || field.find('"') != std::string::npos);
    
    if(!needsQuotes) return field;  // No special chars, return as-is
    
    // Build quoted and escaped field per RFC 4180
    std::string result = "\"";  // Opening quote
    
    for(char c : field){
        if(c == '"'){
            result += "\"\"";  // Escape quotes by doubling them
        } else {
            result += c;  // Keep everything else (including \n and delimiter)
        }
    }
    
    result += "\"";  // Closing quote
    return result;
}

inline std::string Writer::parseString(const std::string& field, char delim) const {
    // Check if field needs quoting (contains delimiter, newline, or quote)
    bool needsQuotes = (field.find(delim) != std::string::npos 
                        || field.find('\n') != std::string::npos 
                        || field.find('"') != std::string::npos);
    
    if(!needsQuotes) return field;  // No special chars, return as-is
    
    // Build quoted and escaped field per RFC 4180
    std::string result = "\"";  // Opening quote
    
    for(char c : field){
        if(c == '"'){
            result += "\"\"";  // Escape quotes by doubling them
        } else {
            result += c;  // Keep everything else (including \n and delimiter)
        }
    }
    
    result += "\"";  // Closing quote
    return result;
}

inline void Writer::writeRow(const std::vector<std::string>& row){
    if(!isOpen()) throw WriterClosedException();
    
    for (size_t i = 0; i < row.size(); ++i) {
        if (i > 0) file << delimiter;  // add delimiter before second+ row
        file << parseString(row[i]);  // ← Parse each string before writing
    }
    file << '\n';  // endl would constantly flush the input buffer, affecting performance
    if (!file.good()) throw WriteLineException(rowNumber, path);
    
    rowNumber++;
    numRows++;
}

inline void Writer::writeRow(const std::vector<std::string>& row, char delim){
    if(!isOpen()) throw WriterClosedException();
    
    for (size_t i = 0; i < row.size(); ++i) {
        if (i > 0) file << delim;  // add delimiter before second+ row
        file << parseString(row[i], delim);  // ← Parse each string before writing
    }
    file << '\n';  // endl would constantly flush the input buffer, affecting performance
    if (!file.good()) throw WriteLineException(rowNumber, path);
    
    rowNumber++;
    numRows++;
}

inline void Writer::writeAll(const table::Table& t){
    if(!isOpen()) throw WriterClosedException();
    for(size_t i = 0; i < t.getHeight(); ++i) writeRow(t[i]);
}

inline void Writer::writeAll(const table::Table& t, char delim){
    if(!isOpen()) throw WriterClosedException();
    for(size_t i = 0; i < t.getHeight(); ++i) writeRow(t[i], delim);
}

inline void Writer::close() noexcept {
    if (file.is_open()) file.close();
    
    rowNumber = 0;          // reset rowNumber counter
    path.clear();      // clear stored file path
}

//  === READERWRITER METHODS ===

inline bool ReaderWriter::isOpen() const noexcept { return (reader.isOpen() && writer.isOpen()); }

inline void ReaderWriter::flush() {
    if(!isOpen()) throw ReaderWriterClosedException();
    writer.flush();
}

inline void ReaderWriter::setHeader(){
    if(!isOpen()) throw ReaderWriterClosedException();
    reader.setHeader();
}

inline void ReaderWriter::setHeader(uint32 rowNumber) {
    if(!isOpen()) throw ReaderWriterClosedException();
    reader.setHeader(rowNumber);
}

inline bool ReaderWriter::isHeaderSet() const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.isHeaderSet();
}

inline char ReaderWriter::getDelimiter() const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.getDelimiter();
}

inline uint32 ReaderWriter::getReaderLine() const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.getRowNumber();
}

inline uint32 ReaderWriter::getNumRows() const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.getNumRows();
}

inline uint32 ReaderWriter::getWriterLine() const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return writer.getRowNumber();
}

inline void ReaderWriter::setReaderLine(uint32 rowNumber) {
    if(!isOpen()) throw ReaderWriterClosedException();
    reader.setRowNumber(rowNumber);
}

inline void ReaderWriter::setDelimiter(char delim) {
    if(!isOpen()) throw ReaderWriterClosedException();
    reader.setDelimiter(delim);
    writer.setDelimiter(delim);
}

inline void ReaderWriter::open(const std::string& filePath, uint32 startLine){ // if not appending to the file, truncate it (overwrite)
    reader.open(filePath, startLine);
    try{
        writer.open(filePath, true);
    }catch(const error::FatalException&){
        reader.close();
        throw;
    }catch(const error::NonFatalException&){
        reader.close();
        throw;
    }
}

inline std::optional<std::vector<std::string>> ReaderWriter::readRow() {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.readRow();
}

inline table::Table ReaderWriter::readAll(){
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.readAll();
}

inline table::Table ReaderWriter::readAll(char delim){
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.readAll(delim);
}

inline void ReaderWriter::skipLines(uint32 count){
    if(!isOpen()) throw ReaderWriterClosedException();
    reader.skipLines(count);
}

inline bool ReaderWriter::isEOF() const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.isEOF();      // check the underlying stream
}

inline void ReaderWriter::writeRow(const std::vector<std::string>& fields){
    if(!isOpen()) throw ReaderWriterClosedException();
    
    writer.writeRow(fields);
    reader.setNumLines(writer.getNumRows());
}

inline void ReaderWriter::writeRow(const std::vector<std::string>& fields, char delim){
    if(!isOpen()) throw ReaderWriterClosedException();
    
    writer.writeRow(fields, delim);
    reader.setNumLines(writer.getNumRows());
}

inline void ReaderWriter::writeAll(const table::Table& t){
    if(!isOpen()) throw ReaderWriterClosedException();
    
    writer.writeAll(t);
    reader.setNumLines(writer.getNumRows());
}

inline void ReaderWriter::writeAll(const table::Table& t, char delim){
    if(!isOpen()) throw ReaderWriterClosedException();
    
    writer.writeAll(t, delim);
    reader.setNumLines(writer.getNumRows());
}

inline std::string ReaderWriter::getFieldByType(const std::vector<std::string>& row, const std::string& columnName) const {
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.getFieldByType(row, columnName);
}

inline std::string ReaderWriter::getFieldByType(uint32 rowNumber, const std::string& columnName){
    if(!isOpen()) throw ReaderWriterClosedException();
    return reader.getFieldByType(rowNumber, columnName);
}

inline void ReaderWriter::close() noexcept {
    reader.close();
    writer.close();
}

// Standalone utility functions

inline uint32 countLines(const std::string& filePath){
    // Count the number of lines in a file
    std::ifstream file(filePath);
    if(!file.is_open()) throw FileOpenFailureException(filePath);
    uint32 lineCount = 0;
    std::string rowNumber;
    while(std::getline(file, rowNumber)) lineCount++;
    return lineCount;
}
    
}