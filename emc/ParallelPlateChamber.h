#pragma once
#include "Vec3.h"

class ParallelPlateChamber
{
public:
	double d;
	double V;

	ParallelPlateChamber(double d, double v) : d(d), V(v)
	{}

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
		return Vec3(-V / d, 0.0, 0.0);
	}

	double Phi(const Vec3& r) const
	{
		if (r.x < 0) return -V;
		if (r.x > d) return 0.0;
		return (d - r.x) * -V / d;
	}
};

