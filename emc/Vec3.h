#pragma once
#include <math.h>

class Vec3
{
public:
	double x;
	double y;
	double z;

	Vec3(double x, double y, double z) : x(x), y(y), z(z)
	{}

	Vec3 operator+(const Vec3& a) const
	{
		return Vec3(x + a.x, y + a.y, z + a.z);
	}

	Vec3 operator-(const Vec3& a) const
	{
		return Vec3(x - a.x, y - a.y, z - a.z);
	}

	Vec3 operator*(double f) const
	{
		return Vec3(x * f, y * f, z * f);
	}

	double Square() const
	{
		return x * x + y * y + z * z;
	}

	double Norm() const
	{
		return sqrt(Square());
	}

	static double Dot(const Vec3& a, const Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.y * b.y;
	}

};

