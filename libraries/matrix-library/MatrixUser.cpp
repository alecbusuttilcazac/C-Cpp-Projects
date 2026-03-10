#include <iostream>
#include "Matrix.hpp"

int main(){
    using namespace la;
    
    Matrix mat({{10, 20, 30},
                    {40, 6757, 60},
                    {70, 80, 90}}); 
    std::cout << "Original Matrix:\n" << mat.toString(1, 0) << std::endl;
    
    std::cout << "Default Matrix:\n" << Matrix(2, 2).toString(1, 0) << std::endl;
    
    std::cout << "Matrix Comparison:" << std::endl;
    std::cout << ((mat == Matrix::identity(3)) ? "Matrices are equal." : "Matrices are not equal.") << std::endl;
    std::cout << ((Matrix::identity(3) == Matrix::identity(3)) ? "Matrices are equal." : "Matrices are not equal.") << "\n" << std::endl;
    
    std::cout << "Transposed Matrix:\n" << mat.transpose().toString(1, 0) << std::endl;
    
    std::cout << "Determinant: " << mat.determinant() << std::endl;
    
    Matrix mat2 = mat.inverse(1e-8);
    
    std::cout << "Column 2 of Inverse Matrix: " << mat2.col(1).toString(8, 0) << std::endl;
    
    std::cout << "Inverse Matrix:\n" << mat2.toString(2, 0) << std::endl;
    
    std::cout << "Product of Matrix and its Inverse:\n" << (mat * mat2).toString(2, 0) << std::endl;
    
    std::cout << "Trace of Matrix: " << mat.trace() << std::endl;
    
    std::cout << "Frobenius Norm of Matrix: " << mat.frobeniusNorm() << std::endl;
    
    std::cout << "Rank of Matrix: " << mat.rank() << std::endl;
    
    return 0;
}
