#include "LegendreFitter.h"

LegendreFitter::LegendreFitter(std::initializer_list<int> theOrders) : orders(theOrders)
{
	dim = orders.size();
	for (int n : orders)
		p.emplace_back(n); // Create a polynomial for each requested order.

	sumPP.ResizeAndClear(dim, dim);
	sumPy.ResizeAndClear(dim);
	workP.ResizeAndClear(dim);
	choleskyP.ResizeAndClear(dim);
	choleskyB.ResizeAndClear(dim);
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
	// Do the YY sum.
	sumYY += y * y;
	count += 1.0;
}

const Matrix& LegendreFitter::SolveFit()
{
	cholesky = sumPP;
	LinearAlgebra::CholeskyDecomposition(cholesky, choleskyP);
	choleskyB = sumPy;
	LinearAlgebra::CholeskySolver(cholesky, choleskyP, choleskyB);
	variance = (sumYY - Matrix::Dot(choleskyB, sumPy)) / (count - orders.size()); // chisquare/DOF
	return choleskyB;
}

double LegendreFitter::FitValue(double z) const
{
	double sum = 0;
	for (int i = 0; i < dim; i++)
		sum += choleskyB(i) * p[i](z);
	return sum;
}

double LegendreFitter::FitError(double z) const
{
	Matrix pvec(dim);
	for (int i = 0; i < dim; i++)
		pvec(i) = p[i](z);
	Matrix pvec2 = pvec;
	LinearAlgebra::CholeskySolver(cholesky, choleskyP, pvec2);
	return sqrt(variance*Matrix::Dot(pvec, pvec2));
}

void LegendreFitter::ParameterError(Matrix& error) const
{
	error.ResizeAndClear(dim);
	Matrix unit(dim);
	for (int i = 0; i < dim; i++)
	{
		unit.Clear();
		unit(i) = 1.0;
		LinearAlgebra::CholeskySolver(cholesky, choleskyP, unit);
		error(i) = sqrt(variance * unit(i));
	}
}

LegendreFitter::MaxVal LegendreFitter::FindMaximum() const
{
	double bestZ = 0.0;
	double bestIons = FitValue(0.0);
	double bestError = FitError(0.0);

	for (int i = 1; i <= 100; i++)
	{
		double z = i / 100.0;
		double val = FitValue(z);
		if (val > bestIons)
		{
			bestZ = z;
			bestIons = val;
			bestError = FitError(z);
		}
	}
	struct MaxVal mv = { bestZ, bestIons, bestError };
	return mv;
}
