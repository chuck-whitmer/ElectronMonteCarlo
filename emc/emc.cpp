#include <iostream>
#include <ostream>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>
#include "emc.h"
#include "PseudoDES.h"
#include "ParallelPlateChamber.h"
#include "SphereInSphere.h"
#include "SphereInSphereFromFile.h"
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

    double minCos = -1.0;

    bool mathematicaOutput = false;

    string shape = "pp";  // Default to ParallelPlates.
    double r1 = .02;
    double r2 = .07;

    string phiFile;
    bool havePhiFile = false;

    double V;
    double lambda;

    if (!ReadArgs(argc, argv)) return 1;

    if (!MaybeGetArgRange("d", d1, d2, dStep)) return 1;
    if (!GetArgRange("nc", Nc1, Nc2, NcStep)) return 1;
    if (!GetArgRange("ni", Ni1, Ni2, NiStep)) return 1;
    if (!GetArgRange("dt", dt1, dt2, dtStep)) return 1;
    if (!MaybeGetArg("ui", Ui)) return 1;
    if (!MaybeGetArg("scatter", minCos)) return 1;
    if (!MaybeGetArg("gamma", gamma)) return 1;
    if (!MaybeGetArg("r1", r1)) return 1;
    if (!MaybeGetArg("r2", r2)) return 1;
    if (!GetArg("reps", reps)) return 1;
    if (!MaybeGetArg("seed", seed)) return 1;
    if (!MaybeGetArg("steps", steps)) return 1;
    if (!MaybeGetBoolArg("showpath", showPath)) return 1;
    if (!MaybeGetBoolArg("mathematica", mathematicaOutput)) return 1;

    MaybeGetArg("shape", shape);
    if (shape != "pp" && shape != "ss" && shape != "ssf" && shape != "cyl")
    {
        printf("Unknown shape %s\n", shape.c_str());
        return 1;
    }
    if (shape == "ssf")
    {
        if (!MaybeGetArg("phifile", phiFile))
        {
            printf("ssf type requires a phiFile\n");
            return 1;
        }
    }

    if (steps == 1 && (Nc1 != Nc2 || Ni1 != Ni2 || d1 != d2 || dt1 != dt2)) steps = 2;

    char buf[128];
    std::vector<string> mathematicaListing;

    fprintf(stderr, "Command line: emc");
    for (int i = 1; i < argc; i++)
        fprintf(stderr, " %s", argv[i]);
    fprintf(stderr, "\n\n");

    PseudoDES rand(1, seed);
    Geometry* geom = nullptr;

    if (shape == "ssf")
    {
        try
        {
            geom = new SphereInSphereFromFile(r1, r2, phiFile);
        }
        catch (std::exception& e)
        {
            fprintf(stderr,"File error: %s\n", e.what());
            return 1;
        }
    }
    
    for (int i = 0; i < steps; i++)
    {
        double Ni = Interpolate(Ni1, Ni2, i, steps, NiStep);
        double Nc = Interpolate(Nc1, Nc2, i, steps, NcStep);
        double d = Interpolate(d1, d2, i, steps, dStep);
        double dt = Interpolate(dt1, dt2, i, steps, dtStep);

        V = Ni * Ui;

        if (shape == "pp")
        {
            delete geom;
            geom = new ParallelPlate(d1, V);
            lambda = d / Nc;
        }
        else if (shape == "ss")
        {
            delete geom;
            geom = new SphereInSphere(r1, r2, V);
            lambda = (r2 - r1) / Nc;
            d = r2 - r1;
        }
        else if (shape == "ssf")
        {
            lambda = (r2 - r1) / Nc;
            geom->SetV(V);
        }

        steady_clock::time_point start = steady_clock::now();
        ElectronRunner run(lambda, Ui, *geom, rand, reps, dt, showPath, minCos);
        steady_clock::time_point end = steady_clock::now();
        steady_clock::duration time = end - start;

        if (mathematicaOutput)
        {
            sprintf_s(buf, sizeof buf, "  {%.3f,%.3f,{%.2f,%.2f}}%s\n", Ni, Nc, run.meanIons, run.errIons, (i<steps-1)?",":"");
            mathematicaListing.emplace_back(buf);
        }
        printf("Ni %.3f Nc %.3f d %.3f dt %.2e ions %.2f +- %.2f  cols %.2f +- %.2f  rte %.5f  V/Nc %.2f  %.2f sec\n",
            Ni, Nc, d, dt,
            run.meanIons, run.errIons, run.meanCols, run.errCols, run.rmsTravelError, V / Nc, time.count() * 1e-9);
    }
    if (mathematicaOutput)
    {
        printf("\n");
        printf("{\n");
        for (string s : mathematicaListing)
            printf(s.c_str());
        printf("}\n");
    }
    delete geom;
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

bool emc::MaybeGetArg(string key, string& s)
{
    if (!HaveArg(key)) return true;
    return GetArg(key, s);
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

bool emc::MaybeGetArgRange(string key, double& e1, double& e2, StepType& t)
{
    if (!HaveArg(key)) return true;
    return GetArgRange(key, e1, e2, t);
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

bool emc::GetArg(string key, string& s)
{
    try
    {
        s = args[key];
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
        if (arg == "--help")
        {
            PrintUsage();
            return false;
        }
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
            PrintUsage();
            success = false;
        }
    }
    return success;
}

void emc::PrintUsage()
{
    printf(usage.c_str());
}

string emc::usage=R"(
emc - Electron Monte Carlo

For help: --help

variable=value
variable=value1,value2 - for linear range
variable=value1,value2R - for linear range
variable=value1,value2L - for logarithmic range

Variables to set:

d            - For parallel plates (meters)
r1           - For sphere in sphere (meters)
r2           - For sphere in sphere (meters)
Nc           - Number of collision lengths
Ni           - Number of ionization lengths
dt           - Euler time step (meters/c)
Ui           - Ionization energy (eV, default 15)
scatter      - Minimum cosine for forward scattering (default 0)
gamma        - Ion to electron creation rate (default 0.02)
reps         - Number of electrons to launch
seed         - Serial random number seed
steps        - Steps for range searching
showpath     - For diagnostics, true or false
mathematica  - To format for Mathematica, true or false
shape        - pp for parallel plates, ss for sphere-in-sphere

Sample command line:

emc shape=pp d=.05 nc=10 ni=10 reps=100 seed=3 scatter=.8 dt=1e-3
)";