#pragma once
#include "Vec3.h"

class Vec7
{
public:
	Vec3 pos;
	Vec3 vel;
	double s;

	Vec7(const Vec3& pos, const Vec3& vel, double s) :pos(pos), vel(vel), s(s)
	{}

	Vec7 operator+(const Vec7& b) const
	{
		Vec3 p = pos + b.pos;
		Vec3 v = vel + b.vel;
		return Vec7(p, v, s + b.s);
	}

	Vec7 operator*(double f) const
	{
		Vec3 p = pos * f;
		Vec3 v = vel * f;
		return Vec7(p, v, f * s);
	}
};
