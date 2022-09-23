#include <iostream>
#include <deque>
#include <stack>
#include "ElectronRunner.h"
#include "Electron.h"

double ElectronRunner::dt = 0.05;

ElectronRunner::ElectronRunner(double lambda, double Ui, ParallelPlateChamber& pp, PseudoDES& rand, int reps)
	: lambda(lambda), Ui(Ui), reps(reps)
{
	std::stack<Electron> stack;

	int nIonizations = 0;

//	for (int iRep = 0; iRep < reps; iRep++)
	{
		stack.emplace(pp.CathodeStart(rand), Vec3::Zero);
		while (!stack.empty())
		{
			// Get the next electron to work on.
			Electron elec = stack.top();
			elec.dt = dt;
			stack.pop();
			// Get the distance to the next collision.
			double s = -lambda * log(1.0 - rand.RandomDouble());
			printf("s = %.7f\n", s);
			// Move the electron.
			printf("about to travel\n");
			elec.Travel(s, pp);
			printf("back from travel\n");
			printf("stack size %d\n", stack.size());
		}




	}

















}
