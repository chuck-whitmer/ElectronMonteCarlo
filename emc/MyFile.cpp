//#include <iostream>
#include <filesystem>

#include "MyFile.h"
#include "Convert.h"

using std::string;
using std::vector;

MyFile::MyFile(const string& filename) : Path(filename)
{
	using std::filesystem::path;

	FileName = path(filename).filename().string();

	exceptions(failbit | badbit);
	open(filename, in, _SH_DENYNO);
}

bool MyFile::AtEof()
{
	// We also need the peek because otherwise getline will throw an error
	// on a blank line.
	return (eof() || peek() == EOF);
}

// ReadLine - Returns the next line from the file.
// Keeps track of the line number - for diagnostics.

bool MyFile::ReadLine(string& line)
{
	if (AtEof())
	{
		line.clear();
		return false;
	}

	std::getline(*this, line);
	LineNumber++;
	return true;
}

// ReadLineOfWords - Return the next line from the file as a vector of words which were separated by blanks.

int MyFile::ReadLineOfWords(vector<string>& v)
{
	string line;
	v.resize(0);
	if (!ReadLine(line)) return 0;

	std::size_t strLength = line.length();
	std::size_t pos = 0;
	const char* blanks = " \t";
	while (pos < strLength)
	{
		std::size_t wordStart = line.find_first_not_of(blanks, pos);
		if (wordStart == string::npos) break;
		pos = line.find_first_of(blanks, wordStart + 1);
		if (pos == string::npos) pos = strLength;
		v.emplace_back(line, wordStart, pos - wordStart);
	}
	return static_cast<int>(v.size());
}

int MyFile::ReadLineOfValues(vector<double>& ee)
{
	vector<string> words(10);
	ReadLineOfWords(words);
	for (string& s : words)
		ee.push_back(Convert::ToDouble(s));
	return static_cast<int>(ee.size());
}

// ReadValues - Read the requested number of doubles from the file.
// They may be broken over serveral lines.
// Any remainder of a line is discarded after reading the requested number of values.
// Throws an exception if it cannot succeed.

void MyFile::ReadValues(double* pe, int n)
{
	int nRead = 0;
	vector<string> words;
	while (nRead < n)
	{
		ReadLineOfWords(words);
		for (int i = 0; i < words.size() && nRead < n; i++)
			pe[nRead++] = Convert::ToDouble(words[i]);
	}
}
