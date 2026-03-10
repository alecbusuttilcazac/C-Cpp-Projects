#pragma once

#include <vector>
#include <iostream>
#include <cctype>
#include <fstream>

// I use my own CSV library.
// It can not read matrices, so I leave that to functions in this file.
#include "csv-library/CSV.hpp"
#include "Utils.hpp"

// ===============================================================================
// ---------------------------< METHOD DECLARATIONS >-----------------------------
// ===============================================================================

inline std::vector<City> readIntegerCSV(const std::string& filePath, uint32 startLine, char delimiter = ',');
inline std::vector<std::vector<double>> readDoubleMatrix(const std::string& filePath, uint32 startLine, uint32 dimension);

// ===============================================================================
// ----------------------------< METHOD DEFINITIONS >-----------------------------
// ===============================================================================

inline std::vector<City> readIntegerCSV(const std::string& filePath, uint32 startLine, char delimiter){
    csv::Reader reader(delimiter);
    
    reader.open(filePath);
    reader.setRowNumber(startLine); // start from data
    
    std::vector<City> cities;
    
    while(auto row = reader.readRow()){
        if((*row)[0] == "EOF") 
			break;
        cities.push_back(City{
            (uint32_t) stoi((*row)[0]),    // City ID
            stod((*row)[1]),    // X Coordinate
            stod((*row)[2])     // Y Coordinate
        });  
    }
    
    reader.close();
    return cities;
}

// Read asymmetric TSP matrix and convert to double
// numCols: after every numCols values processed, start a new matrix row
inline std::vector<std::vector<double>> readDoubleMatrix(const std::string& filePath, uint32 startLine, uint32 dimension){
    std::ifstream file;
    
    file.open(filePath, std::ios::in);
    if(!file.is_open()) throw csv::FileOpenFailureException(filePath);
    
    for(uint32 i = 0; i < startLine; i++) 
        file.ignore(2147483647, '\n');
    
    std::vector<std::vector<double>> matrix;
    std::string str;
    char c;
    bool previousCharIsNumber = false;
    uint32_t valueCounted = 0;  // Track total number of values processed
    
    std::vector<double> row;
    while(file.get(c)){
        
        // If we encounter alphabetic characters (headers like EDGE_WEIGHT_SECTION
        // or an EOF marker), read the token and handle appropriately.
        if(std::isalpha(static_cast<unsigned char>(c))){
            std::string token;
            token.push_back(c);
            // read until whitespace or EOF
            while(file.peek() != EOF && !std::isspace(file.peek())){
                char nc; file.get(nc);
                token.push_back(nc);
            }
            // If token is EOF, break; otherwise skip the rest of the line and continue
            if(token == "EOF") break;
            file.ignore(2147483647, '\n');
            previousCharIsNumber = false;
            str.clear();
            continue;
        }

        if(c == ' '){
            if(!previousCharIsNumber) continue;
            
            // if previous character is number
            row.push_back(std::stod(str));
            previousCharIsNumber = false;
            str.clear();
            valueCounted++;
            
            // Every numCols values, start a new row
            if(valueCounted % dimension == 0){
                matrix.push_back(row);
                row.clear();
            }
            continue;
        }

        else if(c == '\n'){
            if(previousCharIsNumber){
                row.push_back(std::stod(str));
                str.clear();
                previousCharIsNumber = false;
                valueCounted++;
                
                // Every numCols values, start a new row
                if(valueCounted % dimension == 0){
                    matrix.push_back(row);
                    row.clear();
                }
            }
            continue;
        }
        
        else if(std::isdigit(static_cast<unsigned char>(c)) || c == '.'){
            str += c;
            previousCharIsNumber = true;
            continue;
        }
        
        // Any other non-handled character: skip until end of line to be robust
        // std::numeric_limits<std::streamsize>::max() is essentially 'infinity'
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        previousCharIsNumber = false;
        str.clear();
        continue;
    }
    
    return matrix;
}