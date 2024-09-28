#ifndef NYA_MATH_H
#define NYA_MATH_H

#include <cmath>

class NyaVec3 {
public:
	float x, y, z;

	float operator[] (int i) const { return (&x)[i]; }
	float& operator[] (int i) { return (&x)[i]; }
	[[nodiscard]] double Length() const { return std::sqrt(x * x + y * y + z * z); }
	double Normalize() {
		auto len = Length();
		x /= len;
		y /= len;
		z /= len;
		return len;
	}

	[[nodiscard]] NyaVec3 Cross(const NyaVec3& a) const { return { y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x }; }
	[[nodiscard]] float Dot(const NyaVec3& a) const { return x * a.x + y * a.y + z * a.z; }
	NyaVec3 operator+(const NyaVec3& a) const { return { x + a.x, y + a.y, z + a.z }; }
	NyaVec3 operator-(const NyaVec3& a) const { return { x - a.x, y - a.y, z - a.z }; }
	NyaVec3 operator*(const NyaVec3& a) const { return { x * a.x, y * a.y, z * a.z }; }
	NyaVec3 operator/(const NyaVec3& a) const { return { x / a.x, y / a.y, z / a.z }; }
	NyaVec3 operator*(const float a) const { return { x * a, y * a, z * a }; }
	NyaVec3 operator/(const float a) const { return { x / a, y / a, z / a }; }
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

NyaVec3 operator*(const float a, const NyaVec3& b) {
	return { a * b.x, a * b.y, a * b.z };
}
NyaVec3 operator/(const float a, const NyaVec3& b) {
	return { a / b.x, a / b.y, a / b.z };
}

class NyaMat4x4 {
public:
	NyaVec3 x;
	float xw;
	NyaVec3 y;
	float yw;
	NyaVec3 z;
	float zw;
	NyaVec3 p;
	float pw;

	float operator[] (int i) const { return (&x.x)[i]; }
	float& operator[] (int i) { return (&x.x)[i]; }

	void SetIdentity() {
		memset(this,0,sizeof(*this));
		x.x = 1;
		y.y = 1;
		z.z = 1;
		pw = 1;
	}

	NyaMat4x4 Invert() const {
		NyaMat4x4 out;
		out.SetIdentity();
		out.x.x = x.x;
		out.x.y = y.x;
		out.x.z = z.x;
		out.y.x = x.y;
		out.y.y = y.y;
		out.y.z = z.y;
		out.z.x = x.z;
		out.z.y = y.z;
		out.z.z = z.z;
		out.p += out.x * p.x;
		out.p += out.y * p.y;
		out.p += out.z * p.z;
		out.p *= -1;
		return out;
	}
};

NyaVec3 operator*(const NyaMat4x4& a, const NyaVec3& b) {
	NyaVec3 out;
	out.x = a.x.x * b.x + a.y.x * b.y + a.z.x * b.z + a.p.x;
	out.y = a.x.y * b.x + a.y.y * b.y + a.z.y * b.z + a.p.y;
	out.z = a.x.z * b.x + a.y.z * b.y + a.z.z * b.z + a.p.z;
	return out;
}
#endif