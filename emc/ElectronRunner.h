#pragma once
#include "ParallelPlateChamber.h"
#include "PseudoDES.h"

class ElectronRunner
{
	int reps;
	double lambda;
	double Ui;
	double sum = 0.0;
	double sum2 = 0.0;
public:
	static double dt;
	ElectronRunner(double lambda, double Ui, ParallelPlateChamber& pp, PseudoDES& rand, int reps);
};

