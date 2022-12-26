#pragma once
#include <string>
#include <vector>
#include <regex>

class Convert
{
public:
	static int ToInt(const std::string& s);
	static double ToDouble(const std::string& s);
	static float ToFloat(const std::string& s);
	static bool TryParse(const std::string& s, int& i);
	static bool TryParse(const std::string& s, double& e);
	static void WriteDataBlock(std::ostream& out, const double* pe, int n, int cols, const char* fmt, const char* prefix = nullptr);
	static void WriteDataBlock(std::ostream& out, const float* pe, int n, int cols, const char* fmt, const char* prefix = nullptr);
	static int ToInts(const std::string& s, char delimiter, std::vector<int>& v);
	static void LowerCase(std::string& s);
	static void UpperCase(std::string& s);

	static std::regex regexInt;
	static std::regex regexDouble;

private:
	// It's private - nobody can call it!
	Convert() {}
};
