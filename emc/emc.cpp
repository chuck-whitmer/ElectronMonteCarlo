#include <iostream>
#include "emc.h"
#include "PseudoDES.h"
#include "ParallelPlateChamber.h"
#include "ElectronRunner.h"
#include "Electron.h"

int main(int argc, char* argv[])
{
    return emc::main(argc, argv);
}

unordered_map<string, string> emc::args;
double emc::d = 5.0e-2;  // 5 cm
double emc::Ui = 15.0;   // 15 eV
double emc::gamma = 1.0 / 50.0;
double emc::Nc;
double emc::Ni;
int emc::reps;
int emc::seed = 1;
bool emc::showPath = false;

double emc::dt = 1.0;

double emc::V;
double emc::lambda;

int emc::main(int argc, char *argv[])
{
    if (!ReadArgs(argc, argv)) return 1;

    if (!MaybeGetArg("d", d)) return 1;
    if (!MaybeGetArg("ui", Ui)) return 1;
    if (!MaybeGetArg("gamma", gamma)) return 1;
    if (!GetArg("nc", Nc)) return 1;
    if (!GetArg("ni", Ni)) return 1;
    if (!GetArg("reps", reps)) return 1;
    if (!MaybeGetArg("seed", seed)) return 1;
    if (!MaybeGetArg("dt", dt)) return 1;
    if (!MaybeGetBoolArg("showpath", showPath)) return 1;

    lambda = d / Nc;
    V = Ni * Ui;

    printf("V=%.3f  lambda=%.6f  d=%.4f  Nc=%.2f  Ni=%.2f  Ui=%.2f  1/gamma=%.2f  reps=%d  seed=%d  dt=%.2e\n",
        V, lambda, d, Nc, Ni, Ui, 1.0/gamma, reps, seed, dt);

    PseudoDES rand(1, seed);
    ParallelPlateChamber pp(d, V);

    ElectronRunner run(lambda, Ui, pp, rand, reps, dt, showPath);

    printf(" ionizations = %.2f +- %.2f  collisions = %.2f +- %.2f   rms travel error = %.5f  V/Nc = %.2f\n", 
        run.meanIons, run.errIons, run.meanCols, run.errCols, run.rmsTravelError, V/Nc);

    return 1;
}


bool emc::HaveArg(string key)
{
    return args.count(key) == 1;
}

bool emc::MaybeGetArg(string key, double& e)
{
    if (!HaveArg(key)) return true;
    return GetArg(key, e);
}

bool emc::MaybeGetArg(string key, int& e)
{
    if (!HaveArg(key)) return true;
    return GetArg(key, e);
}

bool emc::MaybeGetBoolArg(string key, bool& e)
{
    bool success = true;
    if (HaveArg(key))
    {
        string s = args[key];
        ToLower(s);
        if (s == "true")
            e = true;
        else if (s == "false")
            e = false;
        else
        {
            printf("Bad value for %s\n", key.c_str());
            success = false;
        }
    }
    return success;
}

bool emc::GetArg(string key, double& e)
{
    try
    {
        e = std::stod(args[key]);
        return true;
    }
    catch (...)
    {
        printf("No valid value entered for %s\n", key.c_str());
        return false;
    }
}

bool emc::GetArg(string key, int& e)
{
    try
    {
        e = std::stoi(args[key]);
        return true;
    }
    catch (...)
    {
        printf("No valid value entered for %s\n", key.c_str());
        return false;
    }
}

void emc::ToLower(string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), tolower);
}

bool emc::ReadArgs(int argc, char* argv[])
{
    bool success = true;
    for (int ii = 1; ii < argc; ii++)
    {
        // Look for "key=value"
        string arg(argv[ii]);
        size_t eq = arg.find('=');
        if (eq > 0 && eq < arg.length()-1 && eq != string::npos)
        {
            string key = arg.substr(0, eq);
            ToLower(key);
            string val = arg.substr(eq + 1, string::npos);
            
            if (args.count(key) != 0)
            {
                printf("Parameter %s is defined more than once.\n", key.c_str());
                success = false;
            }
            args[key] = val;
        }
        else
        {
            printf("Bad command line option %s.\n", arg.c_str());
            success = false;
        }
    }
    return success;
}
