#include <iostream>
#include <math.h>
#include "Electron.h"

// Scatter
//  Subtracts some energy and chooses a random scattered direction for the velocity.

void Electron::Scatter(double energyLoss, PseudoDES& rand)
{
	double newKineticEnergy = KineticEnergy() - energyLoss;
	if (newKineticEnergy < 0.0) newKineticEnergy = 0.0;
	double vMagnitude = sqrt(2.0 * newKineticEnergy / m);
	double cosTheta = 2.0 * rand.RandomDouble() - 1.0;
	double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	double phi = 2.0 * pi * rand.RandomDouble();
	velocity = vMagnitude * Vec3(cosTheta, sinTheta * cos(phi), sinTheta * sin(phi));
}

// ForwardScatter
//  Subtracts some energy and chooses a random scattered direction for the velocity.
// Tends to scatter in a forward direction, determined by minCos which would have been -1.0 
// for the isotropic scatter.

void Electron::ForwardScatter(double energyLoss, double minCos, PseudoDES& rand)
{
	double newKineticEnergy = KineticEnergy() - energyLoss;
	if (newKineticEnergy < 0.0)
	{
		velocity = Vec3::Zero;
		return;
	}
	// Remember the original velocity direction as basis1.
	// Construct two normal vectors to form a basis set.
	Vec3 basis1 = velocity * (1.0 / velocity.Norm());
	Vec3 basis2 = basis1.ConstructPerpendicular();
	Vec3 basis3 = Vec3::Cross(basis1, basis2);

	double cosTheta = (1.0 - minCos) * rand.RandomDouble() + minCos; // Rand in range minCos to 1.0
	double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	double phi = 2.0 * pi * rand.RandomDouble();

	double vMagnitude = sqrt(2.0 * newKineticEnergy / m);
	velocity = vMagnitude * (cosTheta*basis1 + (sinTheta*cos(phi))*basis2 + (sinTheta*sin(phi))*basis3);
}

// Travel
//  Moves the electron a total distance sTarget along its path through the E-field.
//  Returns the error from not conserving energy.
// (If this is too large, then dt will need to be smaller.)

double Electron::Travel(double sTarget, Geometry& pp, double dt)
{
	double time = 0.0;
	Vec3 p0 = position;
	Vec3 v0 = velocity;
	double s0 = 0.0;

	double phiStart = -pp.Phi(position);
	double keStart = 0.5 * m * velocity.Square();

	Vec3 p1;
	Vec3 v1;
	double s1;

	if (showPath) PrintLocation(time, p0, v0);

	while (true)
	{
		EulerStep(p0, v0, s0, p1, v1, s1, pp, dt);
		//RungeKuttaStep(p0, v0, s0, p1, v1, s1, pp, dt);
		if (s1 > sTarget) break;
		p0 = p1;
		v0 = v1;
		s0 = s1;
		time += dt;
		if (showPath) PrintLocation(time, p0, v0);
	}
	double dtLast = (sTarget - s0) / (s1 - s0) * dt;
	EulerStep(p0, v0, s0, p1, v1, s1, pp, dtLast);
	//RungeKuttaStep(p0, v0, s0, p1, v1, s1, pp, dtLast);
	time += dtLast;
	if (showPath)
	{
		PrintLocation(time, p1, v1);
		printf("\n");
	}

	position = p1;
	velocity = v1;

	double phiEnd = -pp.Phi(position);
	double keEnd = 0.5 * m * velocity.Square();

	double deltaE = (phiEnd + keEnd) - (phiStart + keStart);
	return deltaE; // The amount by which energy was not conserved.
}

void Electron::EulerStep(const Vec3& pos, const Vec3& vel, double s,
	Vec3& pos1, Vec3& vel1, double& s1, Geometry& pp, double stepDt)
{
	Vec7 x(pos, vel, s);
	//	Vec7 xdot(vel, -1.0 / m * pp.EField(pos), vel.Norm());
	Vec7 x1 = x + f(x, pp) * stepDt;
	pos1 = x1.pos;
	vel1 = x1.vel;
	s1 = x1.s;
}
/*
void Electron::LeapFrogStep(const Vec3& pos0, const Vec3& vel0, double s0,
	Vec3& pos1, Vec3& vel1, double& s1, ParallelPlateChamber& pp, double stepDt)
{
	Vec3 a0(acc(pos0,pp));
	pos1 = pos0 + vel0 * stepDt + a0 * (0.5 * stepDt * stepDt);
	Vec3 a1(acc(pos1, pp));
	vel1 = vel0 + (a0 + a1) * (0.5*stepDt);





	Vec7 x(pos, vel, s);
	//	Vec7 xdot(vel, -1.0 / m * pp.EField(pos), vel.Norm());
	Vec7 x1 = x + f(x, pp) * stepDt;
	pos1 = x1.pos;
	vel1 = x1.vel;
	s1 = x1.s;
}
*/
Vec3 Electron::acc(const Vec3& pos, Geometry& pp)
{
	return Vec3(-1.0 / m * pp.EField(pos));
}

// f - Using the position and E-field info from pp, computes the time
// derivative of the Vec7.

Vec7 Electron::f(Vec7 x, Geometry& pp)
{
	return Vec7(x.vel, -1.0 / m * pp.EField(x.pos), x.vel.Norm());
}

void Electron::RungeKuttaStep(const Vec3& pos, const Vec3& vel, double s,
	Vec3& pos1, Vec3& vel1, double& s1, Geometry& pp, double stepDt)
{
	Vec7 x(pos, vel, s);
	Vec7 k1 = f(x, pp);
	Vec7 k2 = f(x + k1 * (stepDt / 2), pp);
	Vec7 k3 = f(x + k2 * (stepDt / 2), pp);
	Vec7 k4 = f(x + k3 * stepDt, pp);
	Vec7 x1 = x + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (stepDt / 6.0);
	pos1 = x1.pos;
	vel1 = x1.vel;
	s1 = x1.s;
}

void Electron::PrintStatus(double t, Vec3& p, Vec3& v, Geometry& pp)
{
	printf("%8.5f :", t);
	printf(" %10.7f %10.7f %10.7f :", p.x, p.y, p.z);
	printf(" %10.7f %10.7f %10.7f :", v.x, v.y, v.z);
	double ke = 0.5 * m * v.Square();
	double pe = -pp.Phi(p);
	printf(" %.7f %.7f %.7f\n", pe, ke, pe + ke);
}

void Electron::PrintLocation(double t, Vec3& p, Vec3& v)
{
	printf("%8.5f : %18.15f %18.15f %18.15f  %18.15f %18.15f %18.15f\n", t, p.x, p.y, p.z, v.x, v.y, v.z);
}


