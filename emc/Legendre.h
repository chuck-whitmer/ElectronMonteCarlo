#pragma once
#include <vector>

class Legendre
{
	static std::vector<std::vector<double>> knownP;

	std::vector<double> innerP;

public:
	Legendre(int n);

	const std::vector<double>& P() const { return innerP; }
	double eval(double x) const;
};

