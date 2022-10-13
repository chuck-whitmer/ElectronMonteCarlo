#include <iostream>
#include <chrono>
#include <cmath>
#include "emc.h"
#include "PseudoDES.h"
#include "ParallelPlateChamber.h"
#include "ElectronRunner.h"
#include "Electron.h"

using std::chrono::steady_clock;

int main(int argc, char* argv[])
{
    return emc::main(argc, argv);
}

unordered_map<string, string> emc::args;

int emc::main(int argc, char *argv[])
{
    double d1 = 5.0e-2;  // 5 cm
    double d2 = d1;
    StepType dStep = linear;

    double Ui = 15.0;   // 15 eV
    double gamma = 1.0 / 50.0;

    double Nc1;
    double Nc2;
    StepType NcStep = linear;

    double Ni1;
    double Ni2;
    StepType NiStep = linear;

    int reps;
    int seed = 1;
    int steps = 1;
    bool showPath = false;

    double dt1 = 1.0;
    double dt2 = dt1;
    StepType dtStep = linear;

    double V;
    double lambda;

    if (!ReadArgs(argc, argv)) return 1;

    if (!GetArgRange("d", d1, d2, dStep)) return 1;
    if (!GetArgRange("nc", Nc1, Nc2, NcStep)) return 1;
    if (!GetArgRange("ni", Ni1, Ni2, NiStep)) return 1;
    if (!GetArgRange("dt", dt1, dt2, dtStep)) return 1;
    if (!MaybeGetArg("ui", Ui)) return 1;
    if (!MaybeGetArg("gamma", gamma)) return 1;
    if (!GetArg("reps", reps)) return 1;
    if (!MaybeGetArg("seed", seed)) return 1;
    if (!MaybeGetArg("steps", steps)) return 1;
    if (!MaybeGetBoolArg("showpath", showPath)) return 1;

    if (steps == 1 && (Nc1 != Nc2 || Ni1 != Ni2 || d1 != d2 || dt1 != dt2)) steps = 2;

    printf("Command line: emc");
    for (int i = 1; i < argc; i++)
        printf(" %s", argv[i]);
    printf("\n\n");

    PseudoDES rand(1, seed);

    for (int i = 0; i < steps; i++)
    {
        double Ni = Interpolate(Ni1, Ni2, i, steps, NiStep);
        double Nc = Interpolate(Nc1, Nc2, i, steps, NcStep);
        double d = Interpolate(d1, d2, i, steps, dStep);
        double dt = Interpolate(dt1, dt2, i, steps, dtStep);

        lambda = d / Nc;
        V = Ni * Ui;

        ParallelPlateChamber pp(d, V);

        steady_clock::time_point start = steady_clock::now();
        ElectronRunner run(lambda, Ui, pp, rand, reps, dt, showPath);
        steady_clock::time_point end = steady_clock::now();
        steady_clock::duration time = end - start;

        printf("Ni %.3f Nc %.3f d %.3f dt %.2e ions %.2f +- %.2f  cols %.2f +- %.2f  rte %.5f  V/Nc %.2f  %.2f sec\n",
            Ni, Nc, d, dt,
            run.meanIons, run.errIons, run.meanCols, run.errCols, run.rmsTravelError, V / Nc, time.count() * 1e-9);
    }
    return 1;
}

double emc::Interpolate(double e1, double e2, int i, int steps, StepType t)
{
    if (steps == 1) return e1;

    if (t == linear)
    {
        return e1 + i * (e2 - e1) / (steps - 1);
    }
    else
    {
        return e1 * exp(i * std::log(e2/e1) / (steps - 1));
    }
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

bool emc::GetArgRange(string key, double& e1, double& e2, StepType& t)
{
    if (!HaveArg(key)) return false;
    string arg(args[key]);
    if (arg.find(',') != string::npos)
    {
        StepType st = linear;
        size_t len = arg.length();
        char last = arg[len - 1];
        if (last == 'r' || last == 'R')
        {
            arg.resize(len - 1);
        } 
        else if (last == 'l' || last == 'L')
        {
            st = log;
            arg.resize(len - 1);
        }
        size_t commaLoc = arg.find(',');
        string part1 = arg.substr(0, commaLoc);
        string part2 = arg.substr(commaLoc+1);
        try
        {
            e1 = std::stod(part1.c_str());
            e2 = std::stod(part2.c_str());
            t = st;
            return true;
        }
        catch (...)
        {
            printf("Invalid values entered for %s\n", key.c_str());
            return false;
        }
    }
    try
    {
        e1 = std::stod(args[key]);
        e2 = e1;
        t = linear;
        return true;
    }
    catch (...)
    {
        printf("No valid value entered for %s\n", key.c_str());
        return false;
    }
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
