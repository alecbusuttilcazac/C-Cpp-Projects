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
#include <string>
#include <vector>
#include <optional>
#include <functional> // for std::reference_wrapper, std::cref

#include "include/TypeDef.hpp" // Project-specific typedefs
#include "include/Error.hpp"  // Project-specific error handling



namespace table{
    
// Specific non-fatal exceptions
class RowOutOfBoundsException : public error::NonFatalException{
public:
    explicit RowOutOfBoundsException(uint32 rowNumber)
    :   NonFatalException("Row index "+std::to_string(rowNumber)+" out of bounds.")
    {}
    explicit RowOutOfBoundsException(uint32 rowNumber, uint32 numRows)
    :   NonFatalException("Row index "+std::to_string(rowNumber)+" out of bounds for size "+std::to_string(numRows)+".")
    {}
};

class ColumnOutOfBoundsException : public error::NonFatalException{
public:
    explicit ColumnOutOfBoundsException(uint32 columnNumber)
    :   NonFatalException("Column index "+std::to_string(columnNumber)+" out of bounds.")
    {}
    explicit ColumnOutOfBoundsException(uint32 columnNumber, uint32 numColumns)
    :   NonFatalException("Column index "+std::to_string(columnNumber)+" out of bounds for size "+std::to_string(numColumns)+".")
    {}
};

class NoTableHeaderException : public error::NonFatalException{
public:
    explicit NoTableHeaderException()
    :   NonFatalException("Header not set before calling header-related function.")
    {}
    explicit NoTableHeaderException(const std::string& filePath)
    :   NonFatalException("Header for "+filePath+" not set before calling header-related function.")
    {}
    explicit NoTableHeaderException(const std::string& filePath, const std::vector<std::string>& possibleHeader, char delimiter = ',')
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


// Table class for 2D operations
class Table{
private:
    std::vector<std::vector<std::string>> m_table;
    std::vector<std::string> m_header;
    
public:
    // operator[] returns the underlying row by reference (Java-like semantics)
    Table() = default;
    Table(std::vector<std::vector<std::string>> table)
    :   m_table(table)
    {}
    Table(std::vector<std::vector<std::string>> table, std::vector<std::string> header)
    :   m_table(table)
    ,   m_header(header)
    {}
    
    void setTable(const std::vector<std::vector<std::string>>& table);

    // Read-only accessors
    const std::vector<std::vector<std::string>>& view() const noexcept; // optional ref when empty
    std::vector<std::vector<std::string>> copy() const; // explicit copy (may throw)
    
    std::vector<std::string> getHeader() const;
    void setHeader(uint32 rowNumber);
    void setHeader(std::vector<std::string> row);
    
    uint32 getWidth() const;
    uint32 getHeight() const;
    bool isEmpty() const noexcept;
    
    void insertRow(const std::vector<std::string>& row);
    void insertRow(const std::vector<std::string>& row, uint32 rowNumber);
    void removeRow(uint32 rowNumber);
    
    std::optional<std::vector<std::string>> getRow(uint32 rowNumber) const;
    std::string getField(uint32 rowNumber, uint32 columnNumber) const;
    
    std::optional<std::vector<std::vector<std::string>>> transpose(bool includeHeader = true);

    // Ergonomic indexed access: return a proxy that throws on invalid column access
    class RowProxy{
    private:
        Table* m_parent;
        uint32 m_row;
        
    public:
        RowProxy(Table* parent, uint32 row) : m_parent(parent), m_row(row) {}
        std::string& operator[](uint32 col){
            return m_parent->at(m_row, col);
        }
        
        // Assign an entire row (copy)
        RowProxy& operator=(const std::vector<std::string>& rhs){
            m_parent->rowRef(m_row) = rhs;
            return *this;
        }
        
        // Assign an entire row (move)
        RowProxy& operator=(const std::optional<std::vector<std::string>>& rhs){
            if (rhs) m_parent->rowRef(m_row) = *rhs;
            else m_parent->rowRef(m_row).clear();
            return *this;
        }
        
        // Allow converting proxy to a copy of the row
        RowProxy& operator=(std::optional<std::vector<std::string>>&& rhs){
            if (rhs) m_parent->rowRef(m_row) = std::move(*rhs);
            else m_parent->rowRef(m_row).clear();
            return *this;
        }
        
        // Assign an entire row (move)
        RowProxy& operator=(std::vector<std::string>&& rhs){
            m_parent->rowRef(m_row) = std::move(rhs);
            return *this;
        }
        
        // Allow converting proxy to a copy of the row
        operator std::vector<std::string>() const {
            if (auto r = m_parent->getRow(m_row)) return *r;
            return {};
        }
        
        // Allow converting proxy to a reference to the underlying row
        operator std::vector<std::string>&() {
            return m_parent->rowRef(m_row);
        }
        
        size_t size() const {
            if (auto r = m_parent->getRow(m_row)) return r->size();
            return 0;
        }
        
        bool empty() const { return size() == 0; }
    };

    class ConstRowProxy{
    private:
        const Table* m_parent;
        uint32 m_row;
        
    public:
        ConstRowProxy(const Table* p, uint32 r) : m_parent(p), m_row(r) {}
        const std::string& operator[](uint32 col) const{
            return m_parent->at(m_row, col);
        }
        // Allow converting const proxy to a copy of the row
        operator std::vector<std::string>() const {
            if (auto r = m_parent->getRow(m_row)) return *r;
            return {};
        }
        // Allow converting const proxy to a const reference to the underlying row
        operator const std::vector<std::string>&() const {
            return m_parent->rowRef(m_row);
        }
        size_t size() const {
            if (auto r = m_parent->getRow(m_row)) return r->size();
            return 0;
        }
        bool empty() const { return size() == 0; }
    };

    RowProxy operator[](uint32 row){
        if (row >= static_cast<uint32>(m_table.size()))
            throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
        return RowProxy(this, row);
    }

    ConstRowProxy operator[](uint32 row) const{
        if (row >= static_cast<uint32>(m_table.size()))
            throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
        return ConstRowProxy(this, row);
    }

    // Explicit checked accessors
    std::string& at(uint32 row, uint32 col);
    const std::string& at(uint32 row, uint32 col) const;
        
    // Return a copy of the row as a vector
    std::vector<std::string> row(uint32 row) const;

    // Return a reference to the underlying row vector (may throw if out of bounds)
    std::vector<std::string>& rowRef(uint32 row);
    const std::vector<std::string>& rowRef(uint32 row) const;

    // Iterator support so Table can be used in range-for loops
    using iterator = std::vector<std::vector<std::string>>::iterator;
    using const_iterator = std::vector<std::vector<std::string>>::const_iterator;

    auto begin() noexcept -> iterator;
    auto end() noexcept -> iterator;
    auto begin() const noexcept -> const_iterator;
    auto end() const noexcept -> const_iterator;
    auto cbegin() const noexcept -> const_iterator;
    auto cend() const noexcept -> const_iterator;
};


//  === TABLE METHODS ===

inline std::string Table::getField(uint32 rowNumber, uint32 columnNumber) const {
    return m_table[rowNumber][columnNumber];
}

inline void Table::setTable(const std::vector<std::vector<std::string>>& table){
    this->m_table = table;
}

inline const std::vector<std::vector<std::string>>& Table::view() const noexcept {
    return m_table;
}

inline std::vector<std::vector<std::string>> Table::copy() const {
    return m_table;
}
    
// inline uint32 Table::getWidth() const {
//     if(m_header.empty()) throw NoTableHeaderException();
//     return static_cast<uint32>(m_header.size());
// }

inline uint32 Table::getHeight() const {
    return static_cast<uint32>(m_table.size());
}

inline bool Table::isEmpty() const noexcept{
    return m_table.empty();
}

inline void Table::insertRow(const std::vector<std::string>& row){
    m_table.push_back(row); // Increase size by 1
}

inline void Table::insertRow(const std::vector<std::string>& row, uint32 rowNumber){
    if(static_cast<size_t>(rowNumber) > m_table.size()) throw RowOutOfBoundsException(rowNumber);
    if(static_cast<size_t>(rowNumber) == m_table.size()) m_table.push_back(row);
    else m_table.insert(m_table.begin() + static_cast<std::ptrdiff_t>(rowNumber), row);
}

inline void Table::removeRow(uint32 rowNumber){
    if(static_cast<size_t>(rowNumber) >= m_table.size()) throw RowOutOfBoundsException(rowNumber);
    m_table.erase(m_table.begin() + static_cast<std::ptrdiff_t>(rowNumber));
}
    
inline std::vector<std::string> Table::getHeader() const {
    if(m_header.empty()) throw NoTableHeaderException();
    return m_header;
}

inline void Table::setHeader(uint32 rowNumber){
    if (rowNumber >= m_table.size()) throw RowOutOfBoundsException(rowNumber);
    m_header = m_table[rowNumber];
}

inline void Table::setHeader(const std::vector<std::string> row){
    m_header = std::move(row);
}

inline std::optional<std::vector<std::string>> Table::getRow(uint32 rowNumber) const {
    if (m_table.empty()) return std::nullopt;
    if (rowNumber >= static_cast<uint32>(m_table.size())) throw RowOutOfBoundsException(rowNumber);
    return m_table[rowNumber];
}

inline std::vector<std::string>& Table::rowRef(uint32 row){
    if (m_table.empty()) throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
    if (row >= static_cast<uint32>(m_table.size())) throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
    return m_table[row];
}

inline const std::vector<std::string>& Table::rowRef(uint32 row) const{
    if (m_table.empty()) throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
    if (row >= static_cast<uint32>(m_table.size())) throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
    return m_table[row];
}




inline std::string& Table::at(uint32 row, uint32 col){
    if (row >= static_cast<uint32>(m_table.size()))
        throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
    if (col >= m_table[row].size())
        throw ColumnOutOfBoundsException(col, static_cast<uint32>(m_table[row].size()));
    return m_table[row][col];
}

inline const std::string& Table::at(uint32 row, uint32 col) const{
    if (row >= static_cast<uint32>(m_table.size()))
        throw RowOutOfBoundsException(row, static_cast<uint32>(m_table.size()));
    if (col >= m_table[row].size())
        throw ColumnOutOfBoundsException(col, static_cast<uint32>(m_table[row].size()));
    return m_table[row][col];
}

// Iterator implementations
inline auto Table::begin() noexcept -> std::vector<std::vector<std::string>>::iterator {
    return m_table.begin();
}

inline auto Table::end() noexcept -> std::vector<std::vector<std::string>>::iterator {
    return m_table.end();
}

inline auto Table::begin() const noexcept -> std::vector<std::vector<std::string>>::const_iterator {
    return m_table.begin();
}

inline auto Table::end() const noexcept -> std::vector<std::vector<std::string>>::const_iterator {
    return m_table.end();
}

inline auto Table::cbegin() const noexcept -> std::vector<std::vector<std::string>>::const_iterator {
    return m_table.cbegin();
}

inline auto Table::cend() const noexcept -> std::vector<std::vector<std::string>>::const_iterator {
    return m_table.cend();
}

}