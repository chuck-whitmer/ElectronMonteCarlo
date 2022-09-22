#include "Vec3.h"

Vec3 operator*(double f, const Vec3& a)
{
	return Vec3(a.x * f, a.y * f, a.z * f);
}
