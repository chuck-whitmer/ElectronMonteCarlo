#pragma once
#include "ParallelPlateChamber.h"
#include "PseudoDES.h"

class ElectronRunner
{
public:
	double meanIons;
	double errIons;
	double meanCols;
	double errCols;
	int reps;
	double rmsTravelError;

	ElectronRunner(double lambda, double Ui, ParallelPlateChamber& pp, PseudoDES& rand, int reps, 
		double dt, bool showPath, double minCos=-1.0);
};

