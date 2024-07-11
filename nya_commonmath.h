#include <cmath>

class NyaVec3 {
public:
	double x, y, z;

	double operator[] (int i) const { return (&x)[i]; }
	double& operator[] (int i) { return (&x)[i]; }
	[[nodiscard]] double Length() const { return std::sqrt(x * x + y * y + z * z); }
	double Normalize() {
		auto len = Length();
		x /= len;
		y /= len;
		z /= len;
		return len;
	}

	[[nodiscard]] NyaVec3 Cross(const NyaVec3& a) const { return { y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x }; }
	[[nodiscard]] NyaVec3 Dot(const NyaVec3& a) const { return { x * a.x + y * a.y + z * a.z }; }
	NyaVec3 operator+(const NyaVec3& a) const { return { x + a.x, y + a.y, z + a.z }; }
	NyaVec3 operator-(const NyaVec3& a) const { return { x - a.x, y - a.y, z - a.z }; }
	NyaVec3 operator*(const NyaVec3& a) const { return { x * a.x, y * a.y, z * a.z }; }
	NyaVec3 operator/(const NyaVec3& a) const { return { x / a.x, y / a.y, z / a.z }; }
	NyaVec3 operator*(const double a) const { return { x * a, y * a, z * a }; }
	NyaVec3 operator/(const double a) const { return { x / a, y / a, z / a }; }
	NyaVec3 operator-() const { return { -x, -y, -z }; }

	NyaVec3& operator+=(const NyaVec3& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}
	NyaVec3& operator-=(const NyaVec3& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}
	NyaVec3& operator*=(const NyaVec3& a) {
		x *= a.x;
		y *= a.y;
		z *= a.z;
		return *this;
	}
	NyaVec3& operator*=(const double a) {
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}
	NyaVec3& operator/=(const double a) {
		x /= a;
		y /= a;
		z /= a;
		return *this;
	}
	
	[[nodiscard]] inline double length() const { return Length(); } // required naming for spline library
};

NyaVec3 operator*(const double a, const NyaVec3& b) {
	return { a * b.x, a * b.y, a * b.z };
}
NyaVec3 operator/(const double a, const NyaVec3& b) {
	return { a / b.x, a / b.y, a / b.z };
}