#pragma once

#include "include/Error.hpp"
#include <vector>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>

namespace la{

class Matrix {
private:
    size_t rowSize, colSize;
    std::vector<double> data;
    bool isAugmented = false;
    
    // Internal functions
    
    double& get(size_t row, size_t col) {
        return data[row * rowSize + col];
    }
    const double& get(size_t row, size_t col) const {
        return data[row * rowSize + col];
    }
    
    void set(size_t row, size_t col, double value){
        data[row * rowSize + col] = value;
    }
    
    Matrix submatrix(size_t startRow, size_t startColumn, size_t numRows, size_t numColumns){
        Matrix matrix(numRows, numColumns);
        
        for(int i=startRow; i<(startRow+numRows); i++)
            std::copy(data.begin() + i * rowSize + startColumn, 
                      data.begin() + i * rowSize + startColumn + numColumns,
                      matrix.data.begin() + (i - startRow) * numColumns);
        
        return matrix;
    }

    void resize(size_t numRows, size_t numColumns){
        size_t old_colSize = colSize;
        size_t old_rowSize = rowSize;
        
        colSize = numRows;
        rowSize = numColumns;
        
        std::vector<double> old_data = data;
        data.resize(rowSize * colSize, 0.0);
        
        for(size_t y=0; y<std::min(numRows, old_colSize); y++){
            for(size_t x=0; x<std::min(numColumns, old_rowSize); x++){
                data[y * rowSize + x] = old_data[y * old_rowSize + x];
            }
        }
    }

    class RowProxy {
    private:
        std::vector<double>& data;
        size_t rowIndex;
        size_t rowSize;
        
    public:
        RowProxy(std::vector<double>& data, size_t rowIndex, size_t rowSize)
            : data(data), rowIndex(rowIndex), rowSize(rowSize) {}
        
        double& operator[](size_t col) {
            return data[rowIndex * rowSize + col];
        }
        
        const double& operator[](size_t col) const {
            return data[rowIndex * rowSize + col];
        }
    };

public:
    Matrix(size_t rowSize, size_t colSize){
        if (rowSize<1 || colSize<1) throw error::FatalException("Matrix dimensions must be greater than 0");
        this->rowSize = rowSize;
        this->colSize = colSize;
        
        // set data vector size
        data.resize(rowSize * colSize);
        zero();
    }
    
    Matrix(std::vector<std::vector<double>> rows){
        colSize = rows.size();
        rowSize = rows[0].size();
        data.reserve(rowSize * colSize);
        
        for(int y=0; y<colSize; y++){
            if(rows[y].size() != rowSize)
                throw error::FatalException("Mismatching row sizes passed to matrix constructor.");
            
            for(int x=0; x<rowSize; x++)
                data.push_back(rows[y][x]);
        }
    }
    
    
    ~Matrix() = default;
    
    static Matrix identity(size_t size);
    
    RowProxy operator[](size_t row);            // element access using [x][y]
    const RowProxy operator[](size_t row) const;
    
    void zero();
    
    Matrix col(size_t col) const;
    Matrix row(size_t row) const;
    
    size_t getRowSize() const;
    size_t getColSize() const;
    
    double determinant() const;                       // Gaussian elimination O(n³)
    
    friend bool operator==(const Matrix& mtx1, const Matrix& mtx2);
    friend bool operator!=(const Matrix& mtx1, const Matrix& mtx2);
    
    Matrix operator+(const Matrix& other) const;
    void operator+=(const Matrix& other);
    
    Matrix operator+(double scalar) const;
    friend Matrix operator+(double scalar, const Matrix& mtx);
    void operator+=(double scalar);
    
    Matrix operator-(const Matrix& other) const;
    void operator-=(const Matrix& other);
    
    Matrix operator-(double scalar) const;
    friend Matrix operator-(double scalar, const Matrix& mtx);
    void operator-=(double scalar);
    
    Matrix operator*(const Matrix& other) const;
    void operator*=(const Matrix& other);
    
    Matrix operator*(double scalar) const;
    friend Matrix operator*(double scalar, const Matrix& mtx);
    void operator*=(double scalar);
    
    Matrix operator/(const Matrix& other) const;
    void operator/=(const Matrix& other);
    
    Matrix operator/(double scalar) const;
    void operator/=(double scalar);
    
    Matrix square() const;
    void squareInPlace();
    
    // double dotProduct(const Matrix& other) const;
    friend double dotProduct(const Matrix& rowVector, const Matrix& columnVector);
    
    Matrix transpose() const;
    void transposeInPlace();
    
    double trace() const;
    
    Matrix inverse() const;
    Matrix inverse(double threshold) const;
    void inverseInPlace();
    void inverseInPlace(double threshold);
    void operator!();
    
    Matrix augment(const Matrix &other) const;
    void augmentInPlace(const Matrix &other);
    
    Matrix rowEchelonForm() const;  // forward elimination
    void rowEchelonFormInPlace();
    
    Matrix reducedREF();    // calls REF, then back-substitutes
    void reducedREFInPlace();
    
    size_t rank() const;
    
    double frobeniusNorm() const;
    
    Matrix clean(double threshold=1e-10) const;
    void cleanInPlace(double threshold=1e-10);
    bool isBasicallyZero(double value, double threshold) const;
    
    void print() const;
    std::string toString(int precision = 2, int tabAmount = 1) const;
};

bool Matrix::isBasicallyZero(double value, double threshold=1e-10) const {
    return std::abs(value) < threshold;
}

size_t Matrix::rank() const {
    Matrix ref = rowEchelonForm();
    size_t r = 0;
    for(size_t i = 0; i < ref.colSize; i++){           // each row
        bool nonZeroRow = false;
        for(size_t j = 0; j < ref.rowSize; j++){       // each col
            if(!ref.isBasicallyZero(ref.get(i, j))){
                nonZeroRow = true;
                break;
            }
        }
        if(nonZeroRow) r++;
    }
    return r;
}

double Matrix::frobeniusNorm() const{
    double sum = 0.0;
    for(double value : data)
        sum += value * value;
    
    return std::sqrt(sum);
}

Matrix Matrix::clean(double threshold) const{
    Matrix cleanMatrix = *this;
    
    for(double& value : cleanMatrix.data)
        if(isBasicallyZero(value, threshold))
            value = 0.0;
    
    return cleanMatrix;
}

void Matrix::cleanInPlace(double threshold){
    for(double& value : data)
        if(isBasicallyZero(value))
            value = 0.0;
}

bool operator==(const Matrix& mtx1, const Matrix& mtx2){
    if(mtx1.rowSize != mtx2.rowSize || mtx1.colSize != mtx2.colSize)
        return false;
    
    for(size_t i=0; i<mtx1.data.size(); i++)
        if(!mtx1.isBasicallyZero(mtx1.data[i] - mtx2.data[i]))
            return false;
    
    return true;
}

bool operator!=(const Matrix& mtx1, const Matrix& mtx2){
    return !(mtx1 == mtx2);
}

double Matrix::trace() const{
    double sum = 0.0;
    
    for(size_t i=0; i<std::min(rowSize, colSize); i++)
        sum += get(i, i);
    
    return sum;
}

Matrix Matrix::square() const{
    return (*this) * (*this);
}

void Matrix::squareInPlace(){
    (*this) *= (*this);
}

Matrix Matrix::operator+(double scalar) const{
    Matrix returnMatrix(rowSize, colSize);
    
    for(size_t i=0; i<data.size(); i++)
        returnMatrix.data[i] = data[i] + scalar;
        
    return returnMatrix;
}

Matrix operator+(double scalar, const Matrix& mtx){
    return mtx + scalar;
}

void Matrix::operator+=(double scalar){
    for(double& mtxValue : data)
        mtxValue += scalar;
}

Matrix Matrix::operator-(double scalar) const{
    Matrix returnMatrix(rowSize, colSize);
    
    for(size_t i=0; i<data.size(); i++)
        returnMatrix.data[i] = data[i] - scalar;
        
    return returnMatrix;
}

Matrix operator-(double scalar, const Matrix& mtx){
    Matrix returnMatrix(mtx.rowSize, mtx.colSize);
    
    for(size_t i=0; i<mtx.data.size(); i++)
        returnMatrix.data[i] = scalar - mtx.data[i];
        
    return returnMatrix;
}

void Matrix::operator-=(double scalar){
    for(double& mtxValue : data)
        mtxValue -= scalar;
}

Matrix Matrix::operator*(double scalar) const{
    Matrix returnMatrix(rowSize, colSize);
    
    for(size_t i=0; i<data.size(); i++)
        returnMatrix.data[i] = data[i] * scalar;
        
    return returnMatrix;
}

Matrix operator*(double scalar, const Matrix& mtx){
    return mtx * scalar;
}

void Matrix::operator*=(double scalar){
    for(double& mtxValue : data)
        mtxValue *= scalar;
}

Matrix Matrix::operator/(double scalar) const{
    if(isBasicallyZero(scalar))
        throw error::NonFatalException("Division by zero in matrix-scalar division.");
    
    Matrix returnMatrix(rowSize, colSize);
    
    for(size_t i=0; i<data.size(); i++)
        returnMatrix.data[i] = data[i] / scalar;
        
    return returnMatrix;
}

void Matrix::operator/=(double scalar){
    if(isBasicallyZero(scalar))
        throw error::NonFatalException("Division by zero in matrix-scalar division.");
    
    for(double& mtxValue : data)
        mtxValue /= scalar;
}

// double Matrix::dotProduct(const Matrix& columnVector) const{
//     if(rowSize != columnVector.colSize || colSize != 1 || columnVector.rowSize != 1)
//         throw error::NonFatalException("Unable to perform dot product, mismatching vectors.");
    
//     double total = 0.0;
//     for(size_t i=0; i<rowSize; i++)
//         total += data[i] * columnVector.data[i];
    
//     return total;
// }

Matrix Matrix::operator+(const Matrix& other) const {
    if(rowSize != other.rowSize || colSize != other.colSize)
        throw error::NonFatalException("Unable to add matrices, mismatching dimensions.");
    
    Matrix summedMatrix(rowSize, colSize);
    
    for(size_t i=0; i<data.size(); i++)
        summedMatrix.data[i] = data[i] + other.data[i];
    
    return summedMatrix;
}

void Matrix::operator+=(const Matrix& other) {
    if(rowSize != other.rowSize || colSize != other.colSize)
        throw error::NonFatalException("Unable to add matrices, mismatching dimensions");
    
    for(size_t i=0; i<data.size(); i++)
        data[i] += other.data[i];
}

Matrix Matrix::operator-(const Matrix& other) const {
    if(rowSize != other.rowSize || colSize != other.colSize)
        throw error::NonFatalException("Unable to subtract matrices, mismatching dimensions");
    
    Matrix subtractedMatrix(rowSize, colSize);
    
    for(size_t i=0; i<data.size(); i++)
        subtractedMatrix.data[i] = data[i] - other.data[i];
    
    return subtractedMatrix;
}

void Matrix::operator-=(const Matrix& other) {
    if(rowSize != other.rowSize || colSize != other.colSize)
        throw error::NonFatalException("Unable to subtract matrices, mismatching dimensions");
    
    for(size_t i=0; i<data.size(); i++)
        data[i] -= other.data[i];
}

Matrix Matrix::operator*(const Matrix& other) const {
    // For A(m x n) * B(n x p) => this->rowSize (n) must equal other.colSize (n)
    if(rowSize != other.colSize)
        throw error::NonFatalException("Unable to multiply matrices, mismatching dimensions");
    
    // Result has m rows (this->colSize) and p columns (other.rowSize)
    Matrix multipliedMatrix(other.rowSize, colSize);
    
    for(size_t i=0; i<colSize; i++)
        for(size_t j=0; j<other.rowSize; j++)
            multipliedMatrix.set(i, j, dotProduct(row(i), other.col(j)));
    
    return multipliedMatrix;
}

void Matrix::operator*=(const Matrix& other) {
    if(colSize != other.rowSize)
        throw error::NonFatalException("Unable to multiply matrices, mismatching dimensions");
    
    *this = *this * other;
}

Matrix Matrix::operator/(const Matrix& other) const {
    return *this * other.inverse();
}

void Matrix::operator/=(const Matrix& other) {
    *this *= other.inverse();
}

void Matrix::reducedREFInPlace(){
    rowEchelonFormInPlace();
    
    // Back substitution - eliminate above pivots
    size_t minDim = std::min(rowSize, colSize);
    
    // Loop backwards from last pivot to first
    for(int i = minDim - 1; i >= 0; i--){
        for(int k = i - 1; k >= 0; k--){
            double factor = get(k, i);
            
            // Subtract factor * row i from row k
            for(size_t j = i; j < rowSize; j++)
                (*this)[k][j] -= factor * get(i, j);
        }
    }
}

Matrix Matrix::reducedREF(){
    Matrix copy = *this;
    copy.reducedREFInPlace();
    return copy;
}

Matrix Matrix::rowEchelonForm() const{
    Matrix copy = *this;
    copy.rowEchelonFormInPlace();
    return copy;
}

void Matrix::rowEchelonFormInPlace(){
    size_t minDimension = std::min(rowSize, colSize);
    
    for(size_t i=0; i<minDimension; i++){
        double maxVal = 0.0;
        size_t pivotRow = i;
        
        for(size_t j=i; j<colSize; j++){
            double currentValue = get(j, i);
            
            if(std::abs(currentValue) > maxVal){
                maxVal = std::abs(currentValue);
                pivotRow = j;
            }
        }
        
        if(isBasicallyZero(maxVal))
            continue;  // Skip this pivot, move to next
        
        if(i != pivotRow)
            for(size_t j=0; j<rowSize; j++)
                std::swap(get(i, j), get(pivotRow, j));
        
        double pivot = get(i, i);
        for(size_t j=i; j<rowSize; j++)
            set(i, j, get(i, j) / pivot);
        
        // Eliminate below pivot
        for(size_t k=i+1; k<colSize; k++){          // for each row below i
            double factor = get(k, i);                   // value to eliminate in column i
            for(size_t j=i; j<rowSize; j++)        // for each column from i onward
                (*this)[k][j] -= factor * get(i, j); // subtract factor * pivot row
        }
    }
}


Matrix Matrix::identity(size_t size){
    Matrix identity(size, size);
    
    for(int i=0; i<size; i++){
        identity[i][i] = 1.0;
    }
    return identity;
}

Matrix::RowProxy Matrix::operator[](size_t row){
    return RowProxy(data, row, rowSize);
}

const Matrix::RowProxy Matrix::operator[](size_t row) const{
    return RowProxy(const_cast<std::vector<double>&>(data), row, rowSize);
}

void Matrix::operator!(){
    inverseInPlace();
}

void Matrix::zero(){
    for(double& mtxValue : data) mtxValue = 0.0;
}

Matrix Matrix::col(size_t col) const{
    // Column vector has 1 column and 'colSize' rows
    Matrix columnVector(1, colSize);
    
    for(size_t i=0; i<colSize; i++){
        columnVector[i][0] = get(i, col);
    }
    return columnVector;
}

Matrix Matrix::row(size_t row) const{
    // Row vector has 'rowSize' columns and 1 row
    size_t startIdx = row * rowSize;
    Matrix rowVector(rowSize, 1);
    
    std::copy(data.begin() + startIdx, 
              data.begin() + startIdx + rowSize, 
              rowVector.data.begin());
              
    return rowVector;
}

size_t Matrix::getRowSize() const{
    return rowSize;
}

size_t Matrix::getColSize() const{
    return colSize;
}

void Matrix::transposeInPlace(){
    if(rowSize == colSize) // easier transposition if square matrix
        for(int outerIndex=0; outerIndex<rowSize; outerIndex++)
            for(int innerIndex=outerIndex+1; innerIndex<rowSize; innerIndex++)
                std::swap(get(outerIndex, innerIndex), get(innerIndex, outerIndex));
            
    else // if NOT square matrix
        *this = transpose();
}

Matrix Matrix::transpose() const{
    Matrix transposed(colSize, rowSize);
    
    for(int y=0; y<colSize; y++)
        for(int x=0; x<rowSize; x++)
            transposed[y][x] = get(x, y);
    
    return transposed;
}

void Matrix::augmentInPlace(const Matrix &other){
    if(colSize != other.colSize)
        throw error::FatalException("Unable to augment, number of rows don't match.");
    
    size_t old_rowSize = rowSize;
    resize(colSize, old_rowSize + other.rowSize);
    
    for(int y=0; y<colSize; y++){
        for(int x=0; x<other.rowSize; x++){
            set(y, x + old_rowSize, other.get(y, x));
        }
    }
    
    isAugmented = true;
}

Matrix Matrix::augment(const Matrix &other) const{
    if(colSize != other.colSize)
        throw error::FatalException("Unable to augment, number of rows don't match.");
    Matrix augmentedMatrix(rowSize + other.getRowSize(), colSize);
    
    for(int y=0; y<colSize; y++)
        for(int x=0; x<rowSize; x++)
            augmentedMatrix[y][x] = get(y, x);
    
    for(int y=0; y<colSize; y++)
        for(int x=0; x<other.rowSize; x++)
            augmentedMatrix.set(y, rowSize + x, other.get(y, x));
    
    augmentedMatrix.isAugmented = true;
    return augmentedMatrix;
}

// Determinant using Gaussian Elimination with partial pivoting
// Done with AI
double Matrix::determinant() const{
    if(rowSize != colSize)
        throw error::NonFatalException("Determinant only defined for square matrices");
    
    size_t n = rowSize;
    if(n == 1) return data[0];
    if(n == 2) return data[0] * data[3] - data[1] * data[2];
    
    // Create working copy
    std::vector<double> temp = data;
    int swaps = 0;
    
    // Forward elimination with partial pivoting
    for(size_t i = 0; i < n; i++) {
        // Find pivot (max absolute value in column i, rows i to n-1)
        size_t pivotRow = i;
        double maxVal = std::abs(temp[i * n + i]);
        
        for(size_t k = i + 1; k < n; k++) {
            double val = std::abs(temp[k * n + i]);
            if(val > maxVal) {
                maxVal = val;
                pivotRow = k;
            }
        }
        
        // Check for singular matrix
        if(isBasicallyZero(maxVal)) {
            return 0.0;  // Determinant is zero
        }
        
        // Swap rows if needed
        if(pivotRow != i) {
            for(size_t j = 0; j < n; j++) {
                std::swap(temp[i * n + j], temp[pivotRow * n + j]);
            }
            swaps++;
        }
        
        // Eliminate below pivot
        for(size_t k = i + 1; k < n; k++) {
            double factor = temp[k * n + i] / temp[i * n + i];
            
            for(size_t j = i; j < n; j++) {
                temp[k * n + j] -= factor * temp[i * n + j];
            }
        }
    }
    
    // Calculate determinant (product of diagonal)
    double det = 1.0;
    for(size_t i = 0; i < n; i++) {
        det *= temp[i * n + i];
    }
    
    // Apply sign from row swaps
    if(swaps % 2 == 1) {
        det = -det;
    }
    
    return det;
}

Matrix Matrix::inverse() const{
    if(rowSize != colSize)
        throw error::FatalException("Cannot invert a non-square matrix.");
    
    if(isBasicallyZero(determinant()))
        throw error::NonFatalException("Matrix is singular, cannot invert matrix.");
    
    Matrix augmented = augment(identity(colSize));
    augmented.reducedREFInPlace();
    
    return augmented.submatrix(0, rowSize, colSize, rowSize);
}

Matrix Matrix::inverse(double threshold) const{
    if(rowSize != colSize)
        throw error::FatalException("Cannot invert a non-square matrix.");
    
    if(isBasicallyZero(determinant()))
        throw error::NonFatalException("Matrix is singular, cannot invert matrix.");
    
    Matrix augmented = augment(identity(colSize));
    augmented.reducedREFInPlace();
    
    return augmented.submatrix(0, rowSize, colSize, rowSize).clean(threshold);
}

void Matrix::inverseInPlace(){
    *this = inverse();
}

void Matrix::inverseInPlace(double threshold){
    *this = inverse(threshold);
}

void Matrix::print() const{
    std::cout << toString() << std::endl;
}

std::string Matrix::toString(int precision, int tabAmount) const{
    int width = 0;
    
    for(double mtxValue : data){
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << mtxValue;
        std::string s = oss.str();
        
        if(s.length() > width)
            width = s.length();
    }
    
    std::string matrixString;
    
    for(int y=0; y<colSize; y++){
        for(int tab=0; tab<tabAmount; tab++)
            matrixString += '\t';
        matrixString+= "[";
                
        for(int x=0; x<rowSize; x++){
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(precision) << get(y, x);
            std::string s = oss.str();
            
            // Right-align by padding left with spaces
            matrixString += std::string(width - s.length(), ' ') + s;
            
            // Add space between elements (except after last one)
            if(x < rowSize - 1)
                matrixString +=  "  ";
        }
        matrixString += "]\n";
    }
    
    return matrixString;
}

double dotProduct(const Matrix& rowVector, const Matrix& columnVector){
    if(rowVector.rowSize != columnVector.colSize || rowVector.colSize != 1 || columnVector.rowSize != 1)
        throw error::NonFatalException("Unable to perform dot product, mismatching vectors.");
    
    double total = 0.0;
    for(size_t i=0; i<rowVector.rowSize; i++)
        total += rowVector.data[i] * columnVector.data[i];
    
    return total;
}

}