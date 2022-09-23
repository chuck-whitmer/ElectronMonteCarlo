#pragma once
#include <unordered_map>
#include <algorithm>
#include <string>



using std::string;
using std::unordered_map;

class emc
{
	static unordered_map<string, string> args;

	static double d; // meters
	static double Ui; // eV
	static double Nc;
	static double Ni;
	static int reps;
	static int seed;

	static double dt;

	static double V; // Volts
	static double lambda;
	static double gamma;

public:
	static int main(int argc, char* argv[]);

private:
	static bool ReadArgs(int argc, char* argv[]);
	static bool GetArg(string key, double& e);
	static bool GetArg(string key, int& e);
	static bool HaveArg(string key);
	static void ToLower(string& s);
	static bool MaybeGetArg(string key, double& e);
	static bool MaybeGetArg(string key, int& e);

};