#include <cmath>
#include "Vec3.h"

const Vec3 Vec3::Zero(0.0, 0.0, 0.0);

Vec3 operator*(double f, const Vec3& a)
{
	return Vec3(a.x * f, a.y * f, a.z * f);
}

// ConstructPerpendicular
//  Returns a unit vector normal to the original

Vec3 Vec3::ConstructPerpendicular() const
{
	// Three vectors that are obviously perpendicular:
	//  Vec3 candidate1(-y,  x, 0);
	//  Vec3 candidate2( 0, -z, y);
	//  Vec3 candidate3(-z,  0, x);
	// For numeric stability return the one with the largest norm.
	// - Note that you wouldn't want to return (-.00002,.00001,0) 
	//   as the normal to (.00001,.00002,1.0000). The small numbers
	//   could be pure roundoff error.
	// Trick: The largest norm excludes the smallest entry.
	Vec3 v;
	if (abs(z) <= abs(y) && abs(z) <= abs(x))
	{
		// z is smallest entry, don't use it.
		v = Vec3(-y, x, 0.0);
	}
	else if (abs(y) <= abs(x))
	{
		// y is smallest entry, don't use it.
		v = Vec3(-z, 0.0, x);
	}
	else
	{
		// x is smallest entry, don't use it.
		v = Vec3(0.0 , -z, y);
	}
	return v * (1.0 / v.Norm());
}
