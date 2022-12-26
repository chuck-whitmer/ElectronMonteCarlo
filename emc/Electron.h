#pragma once
#include "Vec3.h"
#include "Vec7.h"
#include "ParallelPlateChamber.h"

class Electron
{
public:
	static constexpr double m = 510998.950; // eV
	static constexpr double q = 1.0;        // electron charges
	static constexpr double pi = 3.14159265358979324;

	Vec3 position;      // meters
	Vec3 velocity;      // fraction of c
	bool showPath = false;

	Electron(const Vec3& pos, const Vec3& vel) : position(pos), velocity(vel)
	{}

	double KineticEnergy()
	{
		return 0.5 * m * velocity.Square();
	}

	double Travel(double s, Geometry& pp, double dt);
	void Scatter(double energyLoss, PseudoDES& rand);
	void ForwardScatter(double energyLoss, double minCos, PseudoDES& rand);

private:
	void EulerStep(const Vec3& pos, const Vec3& vel, double s,
		Vec3& pos1, Vec3& vel1, double& s1, Geometry& pp, double stepDt);
	void LeapFrogStep(const Vec3& pos0, const Vec3& vel0, double s0,
		Vec3& pos1, Vec3& vel1, double& s1, Geometry& pp, double stepDt);
	void RungeKuttaStep(const Vec3& pos, const Vec3& vel, double s,
		Vec3& pos1, Vec3& vel1, double& s1, Geometry& pp, double stepDt);
	Vec7 f(Vec7 x, Geometry& pp);
	Vec3 acc(const Vec3& pos, Geometry& pp);
	void PrintStatus(double t, Vec3& p, Vec3& v, Geometry& pp);
	void PrintLocation(double t, Vec3& p, Vec3& v);
};

