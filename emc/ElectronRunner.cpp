#include <iostream>
#include <deque>
#include <stack>
#include "ElectronRunner.h"
#include "Electron.h"


ElectronRunner::ElectronRunner(double lambda, double Ui, Geometry& pp, PseudoDES& rand, int reps, 
	double dt, bool showPath, double minCos)
	: reps(reps)
{
	std::stack<Electron> stack;

	double sumIons = 0.0;
	double sumIons2 = 0.0;
	double sumCols = 0.0;
	double sumCols2 = 0.0;
	int nTravelError = 0;
	double sumTravelError2 = 0.0;

	for (int iRep = 0; iRep < reps; iRep++)
	{
		int nIonizations = 0;
		int nCollisions = 0;

		// Push the first electron.
		stack.emplace(pp.CathodeStart(rand), Vec3::Zero);
		if (showPath) stack.top().showPath = true;

		while (!stack.empty())
		{
			// Get the next electron to work on.
			Electron elec = stack.top(); stack.pop();
			//if (showPath) elec.showPath = true;

			// Run it through the collisions.
			while (true)
			{
				// Get the distance to the next collision.
				double s = -lambda * log(1.0 - rand.RandomDouble());

				// Move the electron a distance s.
				double error = elec.Travel(s, pp, dt);
				nTravelError++;
				sumTravelError2 += error * error;

				// If we have passed the anode, then quit.
				if (pp.OutsideAnode(elec.position)) break;

				// We are at a collision. Ionize or not.
				nCollisions++;
				if (elec.KineticEnergy() >= Ui)
				{
					// Ionize! Put a new electron on the stack.
					stack.emplace(elec.position, Vec3::Zero);
					nIonizations++;
					// Scatter our electron, removing Ui of its energy.
					elec.ForwardScatter(Ui, minCos, rand);
				}
				else
				{
					// Choose a smaller amount of energy to lose.
					double deltaE = (0.1 + 0.4 * rand.RandomDouble()) * Ui;
					elec.ForwardScatter(deltaE, minCos, rand);
				}
			}
		}
		double ions = nIonizations;
		sumIons += ions;
		sumIons2 += ions * ions;
		double cols = nCollisions;
		sumCols += cols;
		sumCols2 += cols * cols;
	}
	meanIons = sumIons / reps;
	double stdDevIons = sqrt(sumIons2 / reps - meanIons * meanIons);
	errIons = stdDevIons / sqrt((double)reps);
	meanCols = sumCols / reps;
	double stdDevCols = sqrt(sumCols2 / reps - meanCols * meanCols);
	errCols = stdDevCols / sqrt((double)reps);
	rmsTravelError = sqrt(sumTravelError2 / nTravelError);
}
