#include <iomanip>
#include <algorithm>
#include <cmath>
#include "Convert.h"

using std::string;
using std::vector;

int Convert::ToInt(const string& s)
{
	static const char* blanks = " \t";
	std::size_t end;
	int i = std::stoi(s, &end);
	if (end != s.find_last_not_of(blanks) + 1)
		throw std::invalid_argument("Bad integer");
	return i;
}

std::regex Convert::regexInt(R"(\s*[-+]?[0-9]+\s*)");

bool Convert::TryParse(const string& s, int& i)
{
	// TryParse should never throw an exception.
	// Use regex to make sure the string is a valid number before calling stoi.
	// This is slow! Don't call from code that loads a library, for example.

	// Note that stoi will return an answer on input like "123foo", and there is an
	// option that will point to the non-matching character on return. 
	// BUT: An input like "foo" will throw an exception! So we need to
	// guarantee success before calling. Minimally we need maybe blanks then a digit or +/- then digit.
	// Manually checking that could be faster than regex if we need speed. The 
	// tradeoff is that we would want to make sure that characters unmatched by stoi
	// will be only terminating blanks.

	std::smatch sm;
	if (!std::regex_match(s, sm, regexInt)) return false;
	i = stoi(s);
	return true;
}

double Convert::ToDouble(const string& s)
{
	static const char* blanks = " \t";
	size_t end;
	double e = std::stod(s, &end);
	size_t strEnd = s.find_last_not_of(blanks) + 1; // points to trimmed end of string.
	if (end != strEnd)
		throw std::invalid_argument("invalid ToDouble argument");
	return e;
}

float Convert::ToFloat(const string& s)
{
	static const char* blanks = " \t";
	std::size_t end;
	float e = std::stof(s, &end);
	if (end != s.find_last_not_of(blanks) + 1)
		throw std::invalid_argument("Bad float");
	return e;
}

std::regex Convert::regexDouble(R"(\s*[-+]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][-+]?[0-9]+)?\s*)");

bool Convert::TryParse(const string& s, double& e)
{
	// TryParse should never throw an exception.
	// Use regex to make sure the string is a valid number before calling stod.
	// This is slow! Don't call from code that loads a library, for example.
	std::smatch sm;
	if (std::regex_match(s, sm, regexDouble))
	{
		e = stod(s);
	}
	else
	{
		return false;
	}
	return true;
}

int Convert::ToInts(const string& s, char delimiter, vector<int>& v)
{
	size_t start = 0;
	size_t i;
	while ((i = s.find(delimiter, start)) != string::npos)
	{
		v.push_back(ToInt(s.substr(start, i - start)));
		start = i + 1;
	}
	v.push_back(ToInt(s.substr(start)));
	return static_cast<int>(v.size());
}

void Convert::WriteDataBlock(std::ostream& out, const double* pe, int n, int cols, const char* fmt, const char* prefix)
{
	char buff[128];
	for (int i = 0; i < n; i += cols)
	{
		int nGroup = cols;
		if (i + cols > n) nGroup = n - i;
		if (prefix != nullptr) out << prefix;
		for (int j = 0; j < nGroup; j++)
		{
			snprintf(buff, sizeof buff, fmt, pe[i + j]);
			out << buff;
		}
		out << '\n'; // HUGE performance improvement over std::endl, which always flushes the cache!
	}
}

void Convert::WriteDataBlock(std::ostream& out, const float* pe, int n, int cols, const char* fmt, const char* prefix)
{
	char buff[128];
	for (int i = 0; i < n; i += cols)
	{
		int nGroup = cols;
		if (i + cols > n) nGroup = n - i;
		if (prefix != nullptr) out << prefix;
		for (int j = 0; j < nGroup; j++)
		{
			snprintf(buff, sizeof buff, fmt, pe[i + j]);
			out << buff;
		}
		out << '\n';
	}
}

void Convert::LowerCase(string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), tolower);
}

void Convert::UpperCase(string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), toupper);
}
