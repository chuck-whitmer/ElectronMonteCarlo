#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <string>
#include "PseudoDES.h"
#include "ParallelPlateChamber.h"

using std::string;
using std::unordered_map;

bool ReadArgs(int argc, char* argv[], unordered_map<string, string>& map);
bool GetArg(string key, unordered_map<string, string> args, double& e);
bool GetArg(string key, unordered_map<string, string> args, int& e);
bool HaveArg(string key, unordered_map<string, string> args);

int main(int argc, char *argv[])
{
    std::unordered_map<string, string> args;
    if (!ReadArgs(argc, argv, args)) return 1;

    double d;
    double V;
    if (!GetArg("d", args, d)) return 1;
    if (!GetArg("v", args, V)) return 1;

    ParallelPlateChamber pp(d, V);

    int reps;
    if (!GetArg("reps", args, reps)) return 1;

    int seed = 1;
    if (HaveArg("seed", args))
    {
        if (!GetArg("seed", args, seed)) return 1;
    }

    PseudoDES rand(1,seed);







    return 1;
}

bool HaveArg(string key, unordered_map<string, string> args)
{
    return args.count(key) == 1;
}


bool GetArg(string key, unordered_map<string, string> args, double& e)
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

bool GetArg(string key, unordered_map<string, string> args, int& e)
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

void ToLower(string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), tolower);
}

bool ReadArgs(int argc, char* argv[], unordered_map<string,string>& map)
{
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
            
            if (map.count(key) != 0)
            {
                printf("Parameter %s is defined more than once.\n", key);
                return false;
            }
            map[key] = val;
        }
        else
        {
            printf("Bad command line option %s.\n", arg);
        }
    }
    return true;
}

