#pragma once
#include <math.h>

class Vec3
{
public:
	double x;
	double y;
	double z;

	Vec3() {}

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

	Vec3 ConstructPerpendicular() const;

	Vec3 Unit() const
	{
		double norm = Norm();
		return Vec3(x / norm, y / norm, z / norm);
	}

	static double Dot(const Vec3& a, const Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.y * b.y;
	}

	static Vec3 Cross(const Vec3& a, const Vec3& b)
	{
		return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	static const Vec3 Zero;

};

Vec3 operator*(double f, const Vec3& a);
