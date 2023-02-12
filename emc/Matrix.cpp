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

Matrix::Matrix(const std::initializer_list<const std::initializer_list<double>> list)
{
	rows = list.size();
	int minCols = INT_MAX;
	int maxCols = 0;
	for (auto l : list)
	{
		int s = l.size();
		if (s > maxCols) maxCols = s;
		if (s < minCols) minCols = s;
	}
	if (minCols != maxCols || minCols < 1)
		throw std::exception("Invalid matrix initializer");
	cols = minCols;
	pe = new double[(long)rows * cols];
	double* p = pe;
	for (auto l : list)
	{
		std::copy(l.begin(),l.end(),p);
		p += cols;
	}
}
