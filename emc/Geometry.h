#pragma once
#include "Vec3.h"
#include "PseudoDES.h"

class Geometry
{
public:
	double V;

	Geometry(double v) : V(v)
	{}

	virtual ~Geometry() {}

	virtual Vec3 CathodeStart(PseudoDES& rand) const = 0;
	virtual bool InsideCathode(const Vec3& r) const = 0;
	virtual bool OutsideAnode(const Vec3& r) const = 0;
	virtual Vec3 EField(const Vec3& r) const = 0;
	virtual double Phi(const Vec3& r) const = 0;
};

