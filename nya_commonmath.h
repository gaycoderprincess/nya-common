#ifndef NYA_MATH_H
#define NYA_MATH_H

#include <cmath>

#ifdef NYA_MATH_NO_TEMPLATES
#define NyaVec3Custom NyaVec3

class NyaVec3Custom {
public:
	float x = 0;
	float y = 0;
	float z = 0;

	float operator[] (int i) const { return (&x)[i]; }
	float& operator[] (int i) { return (&x)[i]; }
#else
template<typename T>
class NyaVec3Custom {
public:
	T x = 0;
	T y = 0;
	T z = 0;

	T operator[] (int i) const { return (&x)[i]; }
	T& operator[] (int i) { return (&x)[i]; }
#endif
	[[nodiscard]] double Length() const { return std::sqrt(x * x + y * y + z * z); }
	[[nodiscard]] double LengthSqr() const { return x * x + y * y + z * z; }
	double Normalize() {
		auto len = Length();
		x /= len;
		y /= len;
		z /= len;
		return len;
	}

	[[nodiscard]] NyaVec3Custom Cross(const NyaVec3Custom& a) const { return { y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x }; }
	[[nodiscard]] double Dot(const NyaVec3Custom& a) const { return x * a.x + y * a.y + z * a.z; }
	NyaVec3Custom operator+(const NyaVec3Custom& a) const { return { x + a.x, y + a.y, z + a.z }; }
	NyaVec3Custom operator-(const NyaVec3Custom& a) const { return { x - a.x, y - a.y, z - a.z }; }
	NyaVec3Custom operator*(const NyaVec3Custom& a) const { return { x * a.x, y * a.y, z * a.z }; }
	NyaVec3Custom operator/(const NyaVec3Custom& a) const { return { x / a.x, y / a.y, z / a.z }; }
	NyaVec3Custom operator*(const double a) const { return NyaVec3Custom(x * a, y * a, z * a); }
	NyaVec3Custom operator/(const double a) const { return { x / a, y / a, z / a }; }
	NyaVec3Custom operator-() const { return { -x, -y, -z }; }

	NyaVec3Custom& operator+=(const NyaVec3Custom& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}
	NyaVec3Custom& operator-=(const NyaVec3Custom& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}
	NyaVec3Custom& operator*=(const NyaVec3Custom& a) {
		x *= a.x;
		y *= a.y;
		z *= a.z;
		return *this;
	}
	NyaVec3Custom& operator*=(const double a) {
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}
	NyaVec3Custom& operator/=(const NyaVec3Custom& a) {
		x /= a.x;
		y /= a.y;
		z /= a.z;
		return *this;
	}
	NyaVec3Custom& operator/=(const double a) {
		x /= a;
		y /= a;
		z /= a;
		return *this;
	}

	[[nodiscard]] inline double length() const { return Length(); } // required naming for spline library
};

class NyaVec4
{
public:
	float x, y, z, w;

	NyaVec4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	explicit NyaVec4(float x)
	{
		this->x = x;
		this->y = x;
		this->z = x;
		this->w = x;
	}

	NyaVec4(NyaVec3Custom<float>& v, float w)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = w;
	}
};

#ifndef NYA_MATH_NO_TEMPLATES
typedef NyaVec3Custom<float> NyaVec3;
typedef NyaVec3Custom<double> NyaVec3Double;
#endif

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

#ifdef NYA_MATH_Z_UP
	static inline bool bZUp = true;
#else
	static inline bool bZUp = false;
#endif

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

	NyaMat4x4 operator*(const float b) {
		NyaMat4x4 out;
		out.x = x * b;
		out.y = y * b;
		out.z = z * b;
		out.p = p * b;

		out.xw = xw * b;
		out.yw = yw * b;
		out.zw = zw * b;
		out.pw = pw * b;

		return out;
	}

	NyaMat4x4& operator*=(const float b) {
		return *this = *this * b;
	}

	// Also translated from GLM
	[[nodiscard]] NyaMat4x4 Invert() const {
		float c0 = z.z * pw - p.z * zw;
		NyaVec3 f0 = NyaVec3(c0, c0, y.z * pw - p.z * yw);
		float f0w = y.z * zw - z.z * yw;

		float c1 = z.y * pw - p.y * zw;
		NyaVec3 f1 = NyaVec3(c1, c1, y.y * pw - p.y * yw);
		float f1w = y.y * zw - z.y * yw;

		float c2 = z.y * p.z - p.y * z.z;
		auto f2 = NyaVec3(c2, c2, y.y * p.z - p.y * y.z);
		auto f2w = y.y * z.z - z.y * y.z;

		float c3 = z.x * pw - p.x * zw;
		auto f3 = NyaVec3(c3, c3, y.x * pw - p.x * yw);
		auto f3w = y.x * zw - z.x * yw;

		float c4 = z.x * p.z - p.x * z.z;
		auto f4 = NyaVec3(c4, c4, y.x * p.z - p.x * y.z);
		auto f4w = y.x * z.z - z.x * y.z;

		float c5 = z.x * p.y - p.x * z.y;
		auto f5 = NyaVec3(c5, c5, y.x * p.y - p.x * y.y);
		auto f5w = y.x * z.y - z.x * y.y;

		NyaMat4x4 vecs;
		vecs.x = NyaVec3(y.x, x.x, x.x);
		vecs.y = NyaVec3(y.y, x.y, x.y);
		vecs.z = NyaVec3(y.z, x.z, x.z);
		vecs.p = NyaVec3(yw, xw, xw);

		NyaMat4x4 inv;
		inv.x = vecs.y * f0 - vecs.z * f1 + vecs.p * f2;
		inv.xw = x.y * f0w - x.z * f1w + xw * f2w;

		inv.y = vecs.x * f0 - vecs.z * f3 + vecs.p * f4;
		inv.yw = x.x * f0w - x.z * f3w + xw * f4w;

		inv.z = vecs.x * f1 - vecs.y * f3 + vecs.p * f5;
		inv.zw = x.x * f1w - x.y * f3w + xw * f5w;

		inv.p = vecs.x * f2 - vecs.y * f4 + vecs.z * f5;
		inv.pw = x.x * f2w - x.y * f4w + x.z * f5w;

		inv.x.y *= -1;
		inv.xw *= -1;
		inv.y.x *= -1;
		inv.y.z *= -1;
		inv.z.y *= -1;
		inv.zw *= -1;
		inv.p.x *= -1;
		inv.p.z *= -1;

		float det = 1.f / ((x.x * inv.x.x + x.y * inv.y.x) + (x.z * inv.z.x + xw * inv.p.x));

		return inv * det;
	}

	// Translated from GLM
	NyaMat4x4 operator*(const NyaMat4x4& a) const {
		NyaMat4x4 tmp;

		tmp.x = x * a.x.x + y * a.x.y + z * a.x.z + p * a.xw;
		tmp.xw = xw * a.x.x + yw * a.x.y + zw * a.x.z + pw * a.xw;

		tmp.y = x * a.y.x + y * a.y.y + z * a.y.z + p * a.yw;
		tmp.yw = xw * a.y.x + yw * a.y.y + zw * a.y.z + pw * a.yw;

		tmp.z = x * a.z.x + y * a.z.y + z * a.z.z + p * a.zw;
		tmp.zw = xw * a.z.x + yw * a.z.y + zw * a.z.z + pw * a.zw;

		tmp.p = x * a.p.x + y * a.p.y + z * a.p.z + p * a.pw;
		tmp.pw = xw * a.p.x + yw * a.p.y + zw * a.p.z + pw * a.pw;

		return tmp;
	}

	NyaMat4x4& operator*=(const NyaMat4x4& b) {
		return *this = *this * b;
	}

	void Rotate(NyaVec3 angle) {
		auto tmp = *this;

		double x1;
		double x2;
		double x3;
		double y1;
		double y2;
		double y3;
		double z1;
		double z2;
		double z3;

		if (bZUp) {
			x1 = std::cos(angle.z) * std::cos(angle.y) - (std::sin(angle.z) * std::sin(angle.x)) * std::sin(angle.y);
			x2 = (std::cos(angle.z) * std::sin(angle.x)) * std::sin(angle.y) + std::sin(angle.z) * std::cos(angle.y);
			x3 = -std::cos(angle.x) * std::sin(angle.y);
			y1 = -std::sin(angle.z) * std::cos(angle.x);
			y2 = std::cos(angle.z) * std::cos(angle.x);
			y3 = std::sin(angle.x);
			z1 = (std::sin(angle.z) * std::sin(angle.x)) * std::cos(angle.y) + std::cos(angle.z) * std::sin(angle.y);
			z2 = std::sin(angle.z) * std::sin(angle.y) - (std::cos(angle.z) * std::sin(angle.x)) * std::cos(angle.y);
			z3 = std::cos(angle.x) * std::cos(angle.y);
		}
		else {
			x1 = std::cos(angle.z) * std::cos(angle.y) - (std::sin(angle.z) * std::sin(angle.x)) * std::sin(angle.y);
			x2 = -std::cos(angle.x) * std::sin(angle.y);
			x3 = (std::cos(angle.z) * std::sin(angle.x)) * std::sin(angle.y) + std::sin(angle.z) * std::cos(angle.y);
			y1 = (std::sin(angle.z) * std::sin(angle.x)) * std::cos(angle.y) + std::cos(angle.z) * std::sin(angle.y);
			y2 = std::cos(angle.x) * std::cos(angle.y);
			y3 = std::sin(angle.z) * std::sin(angle.y) - (std::cos(angle.z) * std::sin(angle.x)) * std::cos(angle.y);
			z1 = -std::sin(angle.z) * std::cos(angle.x);
			z2 = std::sin(angle.x);
			z3 = std::cos(angle.z) * std::cos(angle.x);
		}

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

	static NyaMat4x4 ProjectionMatrix(float screenWidth, float screenHeight, float fov, float farClip, float nearClip) {
		float aspectRatio = screenHeight / screenWidth;
		float f = 1.f/std::tan(fov * 0.5f);
		float q = farClip / (farClip - nearClip);

		NyaMat4x4 out;
		memset(&out, 0, sizeof(NyaMat4x4));

		out.x.x = aspectRatio * f;
		out.y.y = f;
		out.z.z = q;
		out.zw = 1.0f;
		out.p.z = -q * nearClip;

		return out;
	}
};

inline NyaVec4 operator*(const NyaMat4x4& a, const NyaVec4& b)
{
	auto out = NyaVec4(0);
	out.x = a.x.x * b.x + a.y.x * b.y + a.z.x * b.z + a.p.x * b.w;
	out.y = a.x.y * b.x + a.y.y * b.y + a.z.y * b.z + a.p.y * b.w;
	out.z = a.x.z * b.x + a.y.z * b.y + a.z.z * b.z + a.p.z * b.w;
	out.w = a.xw * b.x + a.yw * b.y + a.zw * b.z + a.pw * b.w;
	return out;
}

NyaVec3 operator*(const NyaMat4x4& a, const NyaVec3& b) {
	NyaVec3 out;
	out.x = a.x.x * b.x + a.y.x * b.y + a.z.x * b.z + a.p.x;
	out.y = a.x.y * b.x + a.y.y * b.y + a.z.y * b.z + a.p.y;
	out.z = a.x.z * b.x + a.y.z * b.y + a.z.z * b.z + a.p.z;
	return out;
}
#endif