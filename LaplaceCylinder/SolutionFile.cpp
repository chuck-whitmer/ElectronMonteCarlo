#include "MyFile.h"
#include "SolutionFile.h"

using std::vector;
using std::string;

SolutionFile::SolutionFile(string phiFile)
{
	if (!std::filesystem::exists(phiFile))
		throw std::invalid_argument("Invalid filename");

	MyFile file(phiFile);

	if (!file.ReadLine(titleLine))
		throw std::invalid_argument("Unexpected end of file");

	vector<string> words;
	if (file.ReadLineOfWords(words) != 2 || words[0] != "type" || words[1] != "cylindrical")
		throw std::invalid_argument("Unrecognized file type");
	vector<double> values;
	if (file.ReadLineOfValues(values) != 4
		|| floor(values[0]) != values[0] || values[0] <= 0.0
		|| floor(values[1]) != values[1] || values[1] <= 0.0
		|| values[2] <= 0.0
		|| floor(values[3]) != values[3] || values[3] <= 0.0
		)
		throw std::invalid_argument("Invalid dimension parameters");
	
	nRows = (int)floor(values[0]);
	nCols = (int)floor(values[1]);
	latticeSpacing = values[2];
	nInnerSphere = (int)floor(values[3]);
	
	r1 = (nInnerSphere - 1) * latticeSpacing;
	r2 = (nCols - 1) * latticeSpacing;
	h = (nRows - 1) * latticeSpacing;

	_phi = new double[nRows * nCols];

	file.ReadValues(_phi, nRows * nCols);
	printf("Loaded phi file %s\n", phiFile.c_str());
	printf("%s\n", titleLine.c_str());
	printf("%d x %d array, spacing %.7fm, r1 %.7fm\n\n", nRows, nCols, latticeSpacing, (nInnerSphere-1)*latticeSpacing);

	ComputeSphereBoundary();
	InitializeBoundaries();
}

SolutionFile::SolutionFile(int rows, int cols, int nInner, double a, string title) 
	:nRows(rows), nCols(cols), nInnerSphere(nInner), latticeSpacing(a), filename("NewPhi"), titleLine(title)
{
	_phi = new double[nRows * nCols];

	r1 = (nInnerSphere - 1) * latticeSpacing;
	r2 = (nCols - 1) * latticeSpacing;
	h = (nRows - 1) * latticeSpacing;

	// Initialize phi to -1 inside r1 and 0 at outer boundary.
	ComputeSphereBoundary();
	InitializeBoundaries();

	// Use a 1/r interpolation in between.
	for (int i = 0; i < nRows-1; i++)
	{
		double z = i * latticeSpacing;
		for (int j = jeasy[i]; j < nCols - 1; j++)
		{
			double rho = j * latticeSpacing;
			double r = sqrt(z * z + rho * rho);
			double rEnd;
			if (rho == 0.0)
				rEnd = h;
			else if (z == 0.0)
				rEnd = r2;
			else
				rEnd = fmin(h / z, r2 / rho) * r;
			double b = r1 / (rEnd - r1);
			double a = -rEnd * b;
			Phi(i,j) = a / r + b;
		}
	}
}

void SolutionFile::ComputeSphereBoundary()
{
	jeasy.resize(nCols);
	double r = nInnerSphere - 1;  // 201 -> 200.0
	// We make a smooth circle on the grid points as if we are drawing a graphic circle.
	// We use GIQ (Grid Intersect Quantization) [ref: C. Whitmer for Windows graphics].

	// Do the steep part of the curve. |slope|>=1.
	int i=0;
	while (true)
	{
		double eI = i;
		int jLit = (int)floor(sqrt(r * r - eI * eI) + 0.5);
		jeasy[i] = jLit + 1;
		if (i < jLit)
		{
			i++;  // The stored value is good, we can move on.
		}
		else if (i == jLit)
		{
			i++;   // The stored value is on the diagonal and is good.
			break; // But we are done.
		}
		else
		{
			break; // The stored value will be overwritten.
		}
	}

	// Do the shallow part of the curve. |slope|<1.
	for (; i < nInnerSphere; i++)
	{
		double eI = i;
		// We want the first j with sqrt(r^2-j^2)<i-1/2.
		jeasy[i] = (int)ceil(sqrt(r * r - (eI - 0.5)*(eI - 0.5)));
	}

	for (; i < nRows; i++)
		jeasy[i] = 0;
}


void SolutionFile::InitializeBoundaries()
{
	// Initialize phi to -1 inside r1 and 0 at outer boundary.
	for (int i = 0; i < nRows; i++)
		Phi(i, nCols - 1) = 0.0;
	for (int j = 0; j < nCols; j++)
		Phi(nRows - 1, j) = 0.0;
	for (int i = 0; i < nInnerSphere; i++)
	{
		for (int j = 0; j < jeasy[i]; j++)
		{
			Phi(i, j) = -1.0;
		}
	}
}

double SolutionFile::Iterate()
{
	// Start with the special cases bottom row and left column.
	double errorSum = 0.0;
	errorSum += IterateBottomRow();
	errorSum += IterateLeftColumn();

	// Now do the general case.
	for (int i = 1; i < nRows - 1; i++)
	{
		for (int j = jeasy[i]>0 ? jeasy[i] : 1; j < nCols - 1; j++)
		{
			double newVal = 0.25 * ((1.0 + 0.5 / j) * Phi(i, j + 1) + (1.0 - 0.5 / j) * Phi(i, j - 1) + Phi(i + 1, j) + Phi(i - 1, j));
			double err = newVal - Phi(i, j);
			Phi(i, j) = newVal;
			errorSum += err * err;
		}
	}
	return errorSum;
}

double SolutionFile::IterateBottomRow()
{
	// Bottom row is symmetric up and down.
	double errSum = 0.0;
	for (int j = jeasy[0]; j < nCols-1; j++)
	{
		double newVal = 0.25 * ((1.0 + 0.5/j) * Phi(0, j + 1) + (1.0 - 0.5 / j) * Phi(0, j - 1) + 2.0 * Phi(1, j));
		double err = newVal - Phi(0, j);
		Phi(0, j) = newVal;
		errSum += err * err;
	}
	return errSum;
}

double SolutionFile::IterateLeftColumn()
{
	// Left column is symmetric left and right.
	// No curvature term.
	double errSum = 0.0;
	for (int i = nInnerSphere; i < nRows - 1; i++)
	{
		double newVal = (1.0/6.0) * (4.0 * Phi(i, 1) + Phi(i + 1, 0) + Phi(i - 1, 0));
		double err = newVal - Phi(i, 0);
		Phi(i, 0) = newVal;
		errSum += err * err;
	}
	return errSum;
}

void SolutionFile::Write(string outFileName, string format) const
{
	FILE* file;
	fopen_s(&file,outFileName.c_str(), "w");
	if (file == nullptr)
		throw std::invalid_argument("Can't open output file");
	fprintf(file, "%s\n", titleLine.c_str());
	fprintf(file, "type cylindrical\n");
	fprintf(file, "%d %d %.6f %d\n", nRows, nCols, latticeSpacing, nInnerSphere);
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			if (j != 0) fprintf(file, " ");
			fprintf(file, format.c_str(), _phi[i * nCols + j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}
