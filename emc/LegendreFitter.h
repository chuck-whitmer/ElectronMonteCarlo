#pragma once
#include <vector>
#include <initializer_list>
#include "Legendre.h"
#include "Matrix.h"
#include "LinearAlgebra.h"

class LegendreFitter
{
	std::vector<int> orders;
	std::vector<Legendre> p; // One polynomial for each order.
	Matrix sumPP;
	Matrix sumPy;
	Matrix workP;
	Matrix cholesky;
	Matrix choleskyP;
	Matrix choleskyB; // After we call SolveFit, this contains the solution.
	double sumYY = 0.0;
	double count = 0.0;
	double variance; // After we call SolveFit, this contains the variance estimate.
	int dim;
public:
	LegendreFitter(std::initializer_list<int> theOrders);
	void Accumulate(double z, double y);
	const Matrix& SolveFit();
	void ParameterError(Matrix& error) const;
	const std::vector<int>& Orders() const
	{
		return orders;
	}
	double FitValue(double z) const;
	double FitError(double z) const;
	struct MaxVal { double z; double ions; double error; };
	MaxVal FindMaximum() const;
};

