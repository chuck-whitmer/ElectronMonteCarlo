#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include "Vec3.h"

class SolutionFile
{
	int nRows;               // Cylinder half-height  h=a*(nRows-1)
	int nCols;               // Cylinder radius       r2=a*(nCols-1)
	int nInnerSphere;        // Inner sphere radius   r1=a*(nInnerSphere-1)
	std::string filename;
	std::string titleLine;
	double* _phi = nullptr;  // phi(i,j) -> phi[i*nCols + j].  z = i*a, rho = j*a.

	std::vector<int> jeasy;  // "j-easy"

public:
	double r1, r2, h;
	double latticeSpacing;   // Usually called a.

	SolutionFile(std::string phiFile);
	SolutionFile(int rows, int cols, int nInner, double a, std::string title);
	~SolutionFile()
	{
		delete[] _phi;
	}

	double Iterate();
	double IterateBottomRow();
	double IterateLeftColumn();
	void Write(std::string outFileName, std::string format) const;

	Vec3 EField(const Vec3& pos) const;
	Vec3 EFieldXZ(double rho, double z) const;

	double Phi(const Vec3& r) const;
	double PhiRhoZ(double rho, double z) const;
	double& Phi(int i, int j)
	{
		return _phi[i * nCols + j];
	}
	double Phi(int i, int j) const
	{
		return _phi[i * nCols + j];
	}

private:
	//void IterationSetup();
	void ComputeSphereBoundary(); // Initialize the jeasy vector. On row i, jeasy(i) is the first column outside the inner sphere.
	void InitializeBoundaries();
};

