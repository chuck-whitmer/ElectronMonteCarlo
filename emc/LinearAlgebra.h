#include "Matrix.h"

class LinearAlgebra
{
public:
    static void CholeskyDecomposition(Matrix& mat, Matrix& p);
    static void CholeskySolver(const Matrix& mat, const Matrix& p, Matrix& b);
    static void TestCholesky();
private:
    LinearAlgebra() {}
    static void CholeskyDecomposition(double* mat, double* p, int n);
    static void CholeskySolver(const double* mat, const double* p, int n, double* b);
};
