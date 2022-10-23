#pragma once
#include "Vec3.h"
#include "PseudoDES.h"
#include "Geometry.h"

class SphereInSphere : public Geometry
{
	double a, b;
public:
	double r1,r2;

	SphereInSphere(double r1, double r2, double v) : r1(r1), r2(r2), Geometry(v)
	{
		// Precompute a and b so that: Phi(r) = V * (a/r + b)
		b = r1 / (r2 - r1);
		a = -r2 * b;
		// Note that a/r2+b = 0,
		// and a/r1+b = b(1-r2/r1) = b(r1-r2)/r1 = -1
	}

	Vec3 CathodeStart(PseudoDES& rand) const override
	{
		return Vec3(r1 + (r2-r1) * 1e-9, 0.0, 0.0);
	}

	bool InsideCathode(const Vec3& r) const override
	{
		return r.Norm() < r1;
	}

	bool OutsideAnode(const Vec3& r) const override
	{
		return r.Norm() > r2;
	}

	Vec3 EField(const Vec3& pos) const override
	{
		double rSquare = pos.Square();
		double r = sqrt(rSquare);
		if (r < r1 || r > r2) return Vec3::Zero;
		Vec3 rHat = pos * (1.0 / r);
		return rHat * (V * a / rSquare);
	}

	double Phi(const Vec3& r) const override
	{
		double radius = r.Norm();
		if (radius < r1) return -V;
		if (radius > r2) return 0.0;
		return V * (a/radius + b);
	}
};
