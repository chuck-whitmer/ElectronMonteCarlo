#include <cmath> // Always include cmath when using abs().
#include <iostream>
#include <exception>
#include "LinearAlgebra.h"

// CholeskyDecomposition - Factors a positive definite square matrix.
// On return, the matrix has been replaced with a lower triangular matrix.
// What would be the diagonal elements are returned in p.

void LinearAlgebra::CholeskyDecomposition(Matrix& mat, Matrix& p)
{
    int n = mat.Rows();
    if (mat.Cols() != n || p.Rows() != n)
        throw std::exception("Incompatible matrices");
    CholeskyDecomposition(mat.data(), p.data(), n);
}

void LinearAlgebra::CholeskyDecomposition(double *mat, double *p, int n)
{
  for (int i=0, ni=0; i<n; i++, ni+=n)
  {
    double e;
    for (int j=i, nj=ni; j<n; j++, nj+=n)
    {
      double sum = mat[ni+j];
      for (int k=i-1; k>=0; k--)
        sum -= mat[ni+k]*mat[nj+k];
      if (i==j)
        e = sqrt(abs(sum));
      else
        mat[nj+i] = sum/e;
    }
    p[i] = e;
  }
}

// Solves the inverse problem mat.x = b.
// Input is the decomposed matrix and p vector.
// The b vector is replaced with the answer x.

void LinearAlgebra::CholeskySolver(const Matrix& mat, const Matrix& p, Matrix& b)
{
    int n = mat.Rows();
    if (mat.Cols() != n || p.Rows() != n || b.Rows() != n)
        throw std::exception("Incompatible matrices");
    CholeskySolver(mat.data(), p.data(), n, b.data());
}

void LinearAlgebra::CholeskySolver(const double *mat, const double *p, int n, double *b)
{
  for (int i=0; i<n; i++)
  {
    double sum = b[i];
    for (int k=i-1; k>=0; k--)
      sum -= mat[n*i+k]*b[k];
    b[i] = sum/p[i];
  }
  for (int i=n-1; i>=0; i--)
  {
    double sum = b[i];
    for (int k=i+1; k<n; k++)
      sum -= mat[k*n+i]*b[k];
    b[i] = sum/p[i];
  }
}

void LinearAlgebra::TestCholesky()
{
    Matrix a = {
        {5.0, 2.0, 3.0},
        {2.0, 7.0, 5.0},
        {3.0, 5.0, 6.0}
    };
    Matrix p(3);
    CholeskyDecomposition(a, p);

    printf("Test Cholesky\n");

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            double e = (j < i) ? a(i,j) : (j == i) ? p(i) : 0.0;
            printf(" %10.5f",e);
        }
        printf("\n");
    }
    Matrix b = { {1.0},{2.0},{3.0} }; // A column vector.
    CholeskySolver(a, p, b);

    printf("Solution\n");
    for (int i = 0; i < 3; i++)
    {
        printf(" %10.6f",b(i));
    }
    printf("\n");

    printf("Looking for this:\n%s",
R"(
    2.23607    0.00000    0.00000
    0.89443    2.48998    0.00000
    1.34164    1.52612    1.36783
Solution
  -0.172414  -0.206897   0.758621
)");
    // Compare to Mathematica:
    /*
    a = {{5.0, 2.0, 3.0}, {2.0, 7.0, 5.0}, {3.0, 5.0, 6.0}}; 
    Print[a // MatrixForm];
    Print[CholeskyDecomposition[a] // MatrixForm];
    Print[LinearSolve[a, {1, 2, 3}]];
    */
}
