#include "Vec3.h"

const Vec3 Vec3::Zero(0.0, 0.0, 0.0);

Vec3 operator*(double f, const Vec3& a)
{
	return Vec3(a.x * f, a.y * f, a.z * f);
}
