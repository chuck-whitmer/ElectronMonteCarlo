#pragma once
#include <unordered_map>
#include <algorithm>
#include <string>
#include "SolutionFile.h"

using std::string;
using std::unordered_map;

class lpc
{
	static unordered_map<string, string> args;
	static string usage;
	static SolutionFile* solution;

public:
	static int main(int argc, char* argv[]);

private:
	static bool ReadArgs(int argc, char* argv[]);
	static bool GetArg(string key, double& e);
	static bool GetArg(string key, int& e);
	static bool MaybeGetArg(string key, string& s);
	static bool GetArg(string key, string& s);
	static bool HaveArg(string key);
	static bool MaybeGetArg(string key, double& e);
	static bool MaybeGetArg(string key, int& e);
	static bool MaybeGetBoolArg(string key, bool& e);
	static void PrintUsage();
};