#include "Legendre.h"

using std::vector;

vector<vector<double>> Legendre::knownP{ {1.0}, {0.0, 1.0} };

Legendre::Legendre(int n)
{
	for (int i = knownP.size(); i < n + 1; i++)
	{
		vector<double> newP(i+1,0.0);
		for (int j = 0; j <= i - 2; j++)
			newP[j] = -(i-1) * knownP[i-2][j];
		for (int j = 0; j <= i - 1; j++)
			newP[j+1] += (2*i-1) * knownP[i-1][j];
		for (int j = 0; j <= i; j++)
			newP[j] /= i;
		knownP.push_back(newP);
	}
	innerP = knownP[n];
}

double Legendre::eval(double x) const
{
	int n = innerP.size();
	double sum = innerP[n - 1];
	for (int i = n - 2; i >= 0; i--)
		sum = x * sum + innerP[i];
	return sum;
}

double Legendre::operator()(double x) const
{
	int n = innerP.size();
	double sum = innerP[n - 1];
	for (int i = n - 2; i >= 0; i--)
		sum = x * sum + innerP[i];
	return sum;
}
