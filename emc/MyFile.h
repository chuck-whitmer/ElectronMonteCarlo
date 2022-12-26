#pragma once
#include <string>
#include <vector>
#include <fstream>

class MyFile : public std::ifstream
{
public:
	std::string FileName;
	std::string Path;
	int LineNumber = 0;

	MyFile(const std::string& filename);
	~MyFile() {}

	bool AtEof();
	bool ReadLine(std::string& line);
	int ReadLineOfWords(std::vector<std::string>& v);
	int ReadLineOfValues(std::vector<double>& ee);
	void ReadValues(double* pe, int n);
};

