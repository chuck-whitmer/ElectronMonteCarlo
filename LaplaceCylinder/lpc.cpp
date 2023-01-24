// LaplaceCylinder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include "Convert.h"
#include "lpc.h"

using std::chrono::steady_clock;

int main(int argc, char* argv[])
{
    return lpc::main(argc, argv);
}

unordered_map<string, string> lpc::args;
SolutionFile* lpc::solution = nullptr;

int lpc::main(int argc, char* argv[])
{
    if (!ReadArgs(argc, argv)) return 1;

    try
    {
        string infileName;
        if (HaveArg("infile"))
        {
            GetArg("infile", infileName);
            solution = new SolutionFile(infileName);
        }
        else
        {
            int rows, cols, nInner;
            double a;
            if (!HaveArg("rows") || !GetArg("rows", rows))
                throw std::invalid_argument("No value provided for rows");
            if (!HaveArg("cols") || !GetArg("cols", cols))
                throw std::invalid_argument("No value provided for cols");
            if (!HaveArg("a") || !GetArg("a", a))
                throw std::invalid_argument("No value provided for a");
            if (!HaveArg("ninner") || !GetArg("ninner", nInner))
                throw std::invalid_argument("No value provided for nInner");
            solution = new SolutionFile(rows, cols, nInner, a, "NewFile");
        }
        string outfile = "NewFile.txt";
        int reps = 10;

        if (!MaybeGetArg("reps", reps))
            throw std::invalid_argument("Bad reps argument");
        if (!MaybeGetArg("outfile",outfile))
            throw std::invalid_argument("Bad outfile argument");
        printf("Output file name: %s\n", outfile.c_str());
        double a = solution->latticeSpacing;
        double r2 = solution->r2;
        double h = solution->h;
        double vertexCount = r2 * h / a / a;

        steady_clock::time_point start = steady_clock::now();


        double err2;
        for (int i = 0; i < reps; i++)
        {
            err2 = solution->Iterate();
            if (i % 1000 == 0)
                printf("%10d  err = %10.3e\n", i, sqrt(err2 / vertexCount));
        }
        steady_clock::time_point end = steady_clock::now();
        steady_clock::duration time = end - start;
        printf("% 10d  err = %10.3e  time = %.2f min\n", reps, sqrt(err2 / vertexCount), time.count() * 1e-9 / 60.0);

        solution->Write(outfile, "%14.6e");
    }
    catch (std::exception& e)
    {
        printf("Exception: %s\n", e.what());
    }
    return 0;
}


bool lpc::ReadArgs(int argc, char* argv[])
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
        if (eq > 0 && eq < arg.length() - 1 && eq != string::npos)
        {
            string key = arg.substr(0, eq);
            Convert::LowerCase(key);
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

bool lpc::HaveArg(string key)
{
    return args.count(key) == 1;
}

bool lpc::MaybeGetArg(string key, double& e)
{
    if (!HaveArg(key)) return true;
    return GetArg(key, e);
}

bool lpc::MaybeGetArg(string key, int& e)
{
    if (!HaveArg(key)) return true;
    return GetArg(key, e);
}

bool lpc::MaybeGetArg(string key, string& s)
{
    if (!HaveArg(key)) return true;
    return GetArg(key, s);
}

bool lpc::MaybeGetBoolArg(string key, bool& e)
{
    bool success = true;
    if (HaveArg(key))
    {
        string s = args[key];
        Convert::LowerCase(s);
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

bool lpc::GetArg(string key, double& e)
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

bool lpc::GetArg(string key, string& s)
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

bool lpc::GetArg(string key, int& e)
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

void lpc::PrintUsage()
{
    printf(usage.c_str());
}

string lpc::usage = R"(
lpc - Laplace Cylinder Solver

For help: --help

variable=value

Variables to set:

a            - Lattice step size in meters
r1           - Cathode radius in meters
rows         - Row count of the lattice. Half height of cylinder = a*(rows-1).
cols         - Column count of the lattice. Inner radius of anode = a*(cols-1).

Sample command line:

lpc rows=731 cols=731 a=.0001 r1=.02
)";
