#include "LegendreFitter.h"

LegendreFitter::LegendreFitter(std::initializer_list<int> theOrders) : orders(theOrders)
{
	dim = orders.size();
	for (int n : orders)
		p.emplace_back(n); // Create a polynomial for each requested order.

	sumPP.ResizeAndClear(dim, dim);
	sumPy.ResizeAndClear(dim);
	workP.ResizeAndClear(dim);
}

void LegendreFitter::Accumulate(double z, double y)
{
	int dim = orders.size();
	// Compute all orders of P.
	for (int i = 0; i < dim; i++)
		workP(i) = p[i](z);
	// Do Py sums.
	for (int i = 0; i < dim; i++)
		sumPy(i) += workP(i) * y;
	// Do PP sums.
	for (int i = 0; i < dim; i++)
	{
		for (int j = i; j < dim; j++)
		{
			sumPP(i, j) += workP(i) * workP(j);
			if (i != j)	sumPP(j, i) = sumPP(i, j);
		}
	}
	count += 1.0;
}

const Matrix& LegendreFitter::SolveFit()
{
	cholesky = sumPP;
	LinearAlgebra::CholeskyDecomposition(cholesky.data(), choleskyP.data(), dim);
	choleskyB = sumPy;
	LinearAlgebra::CholeskySolver(cholesky.data(), choleskyP.data(), dim, choleskyB.data());
	return choleskyB;
}

double LegendreFitter::FitValue(double z)
{
	double sum = 0;
	for (Legendre pn : p)
	{
		sum += pn(z);
	}
	return sum;
}

double LegendreFitter::FitError(double z)
{
	Matrix pvec(dim);
	for (int i = 0; i < dim; i++)
		pvec(i) = p[i](z);
	Matrix pvec2 = pvec;
	LinearAlgebra::CholeskySolver(cholesky.data(), choleskyP.data(), dim, pvec2.data());
	return Matrix::Dot(pvec, pvec2);
}
