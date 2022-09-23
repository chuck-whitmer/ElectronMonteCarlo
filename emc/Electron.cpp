#include <iostream>
#include "Electron.h"

void Electron::Travel(double s, ParallelPlateChamber& pp)
{
	double sTraveled = 0.0;
	double time = 0.0;
	Vec3 p0 = position;
	Vec3 v0 = velocity;
	double vNorm;
	Vec3 acceleration;

	PrintStatus(time, p0, v0, pp);

	while (true)
	{
		acceleration = -1.0 / m * pp.EField(p0);
		vNorm = v0.Norm();
		sTraveled += dt * vNorm;
		if (sTraveled > s) break;
		// Euler method.
		p0 = p0 + dt * v0;
		v0 = v0 + dt * acceleration;
		time += dt;
		//printf("dt = %11.3e  sTraveled = %11.3e\n", dt, sTraveled);
		PrintStatus(time, p0, v0, pp);
	}
	double overTime = (sTraveled - s) / vNorm;
	double dtLast = dt - overTime;
	p0 = p0 + dtLast * v0;
	v0 = v0 + dtLast * acceleration;
	time += dtLast;
	PrintStatus(time, p0, v0, pp);

	position = p0;
	velocity = v0;
}

void Electron::PrintStatus(double t, Vec3& p, Vec3& v, ParallelPlateChamber& pp)
{
	printf("%8.5f :", t);
	printf(" %10.7f %10.7f %10.7f :", p.x, p.y, p.z);
	printf(" %10.7f %10.7f %10.7f :", v.x, v.y, v.z);
	double ke = 0.5 * m * v.Square();
	double pe = -pp.Phi(p);
	printf(" %.7f %.7f %.7f\n", pe, ke, pe+ke);
}


