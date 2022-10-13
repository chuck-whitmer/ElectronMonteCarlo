#pragma once
#include <unordered_map>
#include <algorithm>
#include <string>



using std::string;
using std::unordered_map;

class emc
{
	enum StepType {linear, log};

	static unordered_map<string, string> args;


public:
	static int main(int argc, char* argv[]);

private:
	static double Interpolate(double e1, double e2, int i, int steps, StepType t);
	static bool ReadArgs(int argc, char* argv[]);
	static bool GetArg(string key, double& e);
	static bool GetArg(string key, int& e);
	static bool GetArgRange(string key, double& e1, double& e2, StepType& t);
	static bool HaveArg(string key);
	static void ToLower(string& s);
	static bool MaybeGetArg(string key, double& e);
	static bool MaybeGetArg(string key, int& e);
	static bool MaybeGetBoolArg(string key, bool& e);
};