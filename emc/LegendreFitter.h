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
	Matrix choleskyB;
	double count = 0.0;
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
};

