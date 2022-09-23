#pragma once
#include "Vec3.h"
#include "ParallelPlateChamber.h"

class Electron
{
public:
	static constexpr double m = 510998.950; // eV
	static constexpr double q = 1.0;        // electron charges
	Vec3 position;      // meters
	Vec3 velocity;      // fraction of c
	double dt = 0.05;

	Electron(const Vec3& pos, const Vec3& vel) : position(pos), velocity(vel)
	{}

	void Travel(double s, ParallelPlateChamber& pp);

private:
	void PrintStatus(double t, Vec3& p, Vec3& v, ParallelPlateChamber& pp);
};

