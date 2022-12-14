#include "MyFile.h"
#include "Cylinder.h"

using std::vector;

Cylinder::Cylinder(double r1, double r2, double h, string phiFile) : r1(r1), r2(r2), h(h), Geometry(1.0)
{
	if (!std::filesystem::exists(phiFile))
		throw std::invalid_argument("Invalid filename");

	MyFile file(phiFile);

	if (!file.ReadLine(title))
		throw std::invalid_argument("Unexpected end of file");

	vector<string> words;
	if (file.ReadLineOfWords(words) != 2 || words[0] != "type" || words[1] != "cylindrical")
		throw std::invalid_argument("Unrecognized file type");
	vector<double> values;
	if (file.ReadLineOfValues(values) != 3
		|| floor(values[0]) != values[0] || values[0] <= 0.0
		|| floor(values[1]) != values[1] || values[1] <= 0.0
		|| values[2] <= 0.0
		)
		throw std::invalid_argument("Invalid dimension parameters");
	nRows = (int)floor(values[0]);
	nCols = (int)floor(values[1]);
	latticeSpacing = values[2];

	_phi = new double[nRows * nCols];

	file.ReadValues(_phi, nRows * nCols);
	printf("Loaded phi file %s\n", phiFile.c_str());
	printf("%s\n", title.c_str());
	printf("%d x %d array, spacing %.7fm\n\n", nRows, nCols, latticeSpacing);
}

double Cylinder::phiVertex(int i, int j) const
{
	// For convenience we allow asking for phiVertex[i,j] even with negative coordinates.
	// We have reflective symmetry on the x and z axes.
	i = abs(i);
	j = abs(j);
	// Past the bounds of the array, we are in the anode.
	if (i >= nRows) return 0.0;
	if (j >= nCols) return 0.0;
	return _phi[i * nCols + j];
}

double Cylinder::Phi(const Vec3& r) const
{
	double rho = sqrt(r.x * r.x + r.y * r.y);
	return PhiRhoZ(rho, r.z);
}

double Cylinder::PhiRhoZ(double rho, double z) const
{
	double rhoScaled = rho / latticeSpacing;
	double zScaled = z / latticeSpacing;
	int i = (int)floor(rhoScaled);
	double alpha = rhoScaled - floor(rhoScaled);
	int j = (int)floor(zScaled);
	double beta = zScaled - floor(zScaled);
	return V * ((1.0 - beta) * (1.0 - alpha) * phiVertex(i, j)
		+ (1.0 - beta) * alpha * phiVertex(i + 1, j)
		+ beta * (1.0 - alpha) * phiVertex(i, j + 1)
		+ beta * alpha * phiVertex(i + 1, j + 1));
}

Vec3 Cylinder::EField(const Vec3& pos) const
{
	double rho = sqrt(pos.x * pos.x + pos.y * pos.y);
	//double phi = PhiRhoZ(rho, pos.z); // !!!
	//printf("pos: %9.6f %9.6f %9.6f  r: %10.4e  ", pos.x, pos.y, pos.z, pos.Norm());

	if (InsideCathode(pos) || OutsideAnode(pos))
	{
		//printf("E\xF9R: %11.4e  ExR: %11.4e  \xED: %10.3e\n", 0.0, 0.0, phi);
		return Vec3::Zero;
	}

	Vec3 Exz = EFieldXZ(rho, pos.z);
	if (rho == 0)
	{
		//printf("E\xF9R: %11.4e  ExR: %11.4e  \xED: %10.3e\n", Exz.z, 0.0, phi);
		return Vec3(0.0, 0.0, Exz.z);
	}
	Vec3 E(pos.x / rho * Exz.x, pos.y / rho * Exz.x, Exz.z);

	//Vec3 rHat = pos.Unit();

	//printf("E\xF9R: %11.4e  ExR: %11.4e  \xED: %10.3e\n", Vec3::Dot(E,rHat), Vec3::Cross(E,rHat).Norm(), phi);

	return E;
}

Vec3 Cylinder::EFieldXZ(double x, double z) const
{
	double xScaled = x / latticeSpacing;
	double zScaled = z / latticeSpacing;
	int i = (int)floor(xScaled);
	double alpha = xScaled - floor(xScaled);
	int j = (int)floor(zScaled);
	double beta = zScaled - floor(zScaled);

	double Ex = -V / latticeSpacing * ((1.0 - beta) * (phiVertex(i + 1, j) - phiVertex(i, j))
		+ beta * (phiVertex(i + 1, j + 1) - phiVertex(i, j + 1)));
	double Ez = -V / latticeSpacing * ((1.0 - alpha) * (phiVertex(i, j + 1) - phiVertex(i, j))
		+ alpha * (phiVertex(i + 1, j + 1) - phiVertex(i + 1, j)));
	return Vec3(Ex, 0.0, Ez);
}

