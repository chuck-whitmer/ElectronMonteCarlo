#pragma once
#include "Vec3.h"
#include "PseudoDES.h"
#include "Geometry.h"

class ParallelPlate : public Geometry
{
public:
	double d;

	ParallelPlate(double d, double v) : d(d), Geometry(v)
	{}

	Vec3 CathodeStart(PseudoDES& rand) const
	{
		return Vec3(d * 1e-9, 0.0, 0.0);
	}

	bool InsideCathode(const Vec3& r) const
	{
		return r.x < 0.0;
	}

	bool OutsideAnode(const Vec3& r) const
	{
		return r.x > d;
	}

	Vec3 EField(const Vec3& r) const
	{
		if (r.x < 0.0 || r.x > d) return Vec3::Zero;
		return Vec3(-V / d, 0.0, 0.0);
	}

	double Phi(const Vec3& r) const
	{
		if (r.x < 0) return -V;
		if (r.x > d) return 0.0;
		return (d - r.x) * -V / d;
	}
};
