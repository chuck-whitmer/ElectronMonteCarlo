#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include "Geometry.h"

using std::string;

class Cylinder : public Geometry
{
	int nRows, nCols;
	double latticeSpacing;
	string title;
	double* _phi = nullptr;

public:
	double r1, r2, h;

	Cylinder(string phiFile);
	~Cylinder()
	{
		delete[] _phi;
	}

	Vec3 CathodeStart(PseudoDES& rand) const override;

	bool InsideCathode(const Vec3& r) const override
	{
		return r.Norm() < r1;
	}

	bool OutsideAnode(const Vec3& r) const override
	{
		return (r.x * r.x + r.y * r.y > r2 * r2 || r.z > h || r.z < -h);
	}

	Vec3 EField(const Vec3& pos) const override;
	Vec3 EFieldXZ(double rho, double z) const;

	double Phi(const Vec3& r) const override;
	double PhiRhoZ(double rho, double z) const;
	double phiVertex(int i, int j) const;
};

