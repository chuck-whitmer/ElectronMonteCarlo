#pragma once
#include <vector>

class Polynomial
{
	std::vector<double> poly;
public:
	Polynomial(const std::vector<double>& v) : poly(v) {}
	Polynomial(int n) : poly(n) {}

	double Evaluate(double x) const
	{
		double xPower = 1.0;
		double sum = poly[0];
		for (int i = 1; i < poly.size(); i++)
		{
			xPower *= x;
			sum += xPower * poly[i];
		}
		return sum;
	}
};
