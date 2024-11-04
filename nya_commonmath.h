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

	NyaMat4x4() {
		SetIdentity();
	}

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

	NyaMat4x4 operator*(const NyaMat4x4& a) const {
		NyaMat4x4 tmp;
		tmp.x.x = x.x * a.x.x + y.x * a.x.y + z.x * a.x.z;
		tmp.x.y = x.y * a.x.x + y.y * a.x.y + z.y * a.x.z;
		tmp.x.z = x.z * a.x.x + y.z * a.x.y + z.z * a.x.z;
		tmp.y.x = x.x * a.y.x + y.x * a.y.y + z.x * a.y.z;
		tmp.y.y = x.y * a.y.x + y.y * a.y.y + z.y * a.y.z;
		tmp.y.z = x.z * a.y.x + y.z * a.y.y + z.z * a.y.z;
		tmp.z.x = x.x * a.z.x + y.x * a.z.y + z.x * a.z.z;
		tmp.z.y = x.y * a.z.x + y.y * a.z.y + z.y * a.z.z;
		tmp.z.z = x.z * a.z.x + y.z * a.z.y + z.z * a.z.z;
		tmp.p.x = x.x * a.p.x + y.x * a.p.y + z.x * a.p.z + p.x;
		tmp.p.y = x.y * a.p.x + y.y * a.p.y + z.y * a.p.z + p.y;
		tmp.p.z = x.z * a.p.x + y.z * a.p.y + z.z * a.p.z + p.z;
		return tmp;
	}

	NyaMat4x4& operator*=(const NyaMat4x4& b) {
		return *this = *this * b;
	}

	void Rotate(NyaVec3 angle) {
		auto tmp = *this;

		double x1 = std::cos(angle.z) * std::cos(angle.y) - (std::sin(angle.z) * std::sin(angle.x)) * std::sin(angle.y);
		double x2 = -std::cos(angle.x) * std::sin(angle.y);
		double x3 = (std::cos(angle.z) * std::sin(angle.x)) * std::sin(angle.y) + std::sin(angle.z) * std::cos(angle.y);
		double y1 = (std::sin(angle.z) * std::sin(angle.x)) * std::cos(angle.y) + std::cos(angle.z) * std::sin(angle.y);
		double y2 = std::cos(angle.x) * std::cos(angle.y);
		double y3 = std::sin(angle.z) * std::sin(angle.y) - (std::cos(angle.z) * std::sin(angle.x)) * std::cos(angle.y);
		double z1 = -std::sin(angle.z) * std::cos(angle.x);
		double z2 = std::sin(angle.x);
		double z3 = std::cos(angle.z) * std::cos(angle.x);

		x.x = x1 * tmp.x.x + y1 * tmp.x.y + z1 * tmp.x.z;
		x.y = x2 * tmp.x.x + y2 * tmp.x.y + z2 * tmp.x.z;
		x.z = x3 * tmp.x.x + y3 * tmp.x.y + z3 * tmp.x.z;
		y.x = x1 * tmp.y.x + y1 * tmp.y.y + z1 * tmp.y.z;
		y.y = x2 * tmp.y.x + y2 * tmp.y.y + z2 * tmp.y.z;
		y.z = x3 * tmp.y.x + y3 * tmp.y.y + z3 * tmp.y.z;
		z.x = x1 * tmp.z.x + y1 * tmp.z.y + z1 * tmp.z.z;
		z.y = x2 * tmp.z.x + y2 * tmp.z.y + z2 * tmp.z.z;
		z.z = x3 * tmp.z.x + y3 * tmp.z.y + z3 * tmp.z.z;
	}

	static NyaMat4x4 LookAt(const NyaVec3& dir, const NyaVec3 up = {0,1,0}) {
		NyaMat4x4 out;
		out.x = up.Cross(dir);
		out.x.Normalize();
		out.y = dir.Cross(out.x);
		out.y.Normalize();
		out.z = dir;
		out.z.Normalize();
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