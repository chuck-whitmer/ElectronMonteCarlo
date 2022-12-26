#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <stdexcept>

using std::string;
#include "Geometry.h"

class SphereInSphereFromFile : public Geometry
{
	int nRows, nCols;
	double latticeSpacing;
	string title;
	double* _phi = nullptr;

public:
	double r1, r2;

	SphereInSphereFromFile(double r1, double r2, string phiFile);
	~SphereInSphereFromFile()
	{
		delete[] _phi;
	}

	Vec3 CathodeStart(PseudoDES& rand) const override
	{
		return Vec3(r1 + (r2 - r1) * 1e-9, 0.0, 0.0);
	}

	bool InsideCathode(const Vec3& r) const override
	{
		return r.Norm() < r1;
	}

	bool OutsideAnode(const Vec3& r) const override
	{
		return r.Norm() > r2;
	}

	Vec3 EField(const Vec3& pos) const override;
	Vec3 EFieldXZ(double rho, double z) const;

	double Phi(const Vec3& r) const override;
	double PhiRhoZ(double rho, double z) const;
	double phiVertex(int i, int j) const;
};

