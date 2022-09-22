#pragma once
#include "Vec3.h"

class Electron
{
public:
	static constexpr double m = 510998.950; // eV
	static constexpr double q = 1.0;        // electron charges
	Vec3 position;      // meters
	Vec3 velocity;      // fraction of c

	Electron(const Vec3& pos, const Vec3& vel) : position(pos), velocity(vel)
	{}
};

