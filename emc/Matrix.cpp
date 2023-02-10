#include <exception>
#include <algorithm>
#include <cmath>
#include "Matrix.h"

double Matrix::Dot(const Matrix& m1, const Matrix& m2)
{
	int dim1 = std::max(m1.rows, m1.cols);
	if (std::min(m1.rows,m1.cols)!=1 || std::min(m2.rows, m2.cols) != 1
		|| dim1 != std::max(m2.rows, m2.cols))
		throw std::exception("Matrix dimensions do not match");
	double sum = 0.0;
	for (int i = 0; i < dim1; i++)
		sum += m1.pe[i] * m2.pe[i];
	return sum;
}

