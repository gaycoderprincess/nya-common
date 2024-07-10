class NyaVec3 {
public:
	float x, y, z;

	float operator[] (int i) const { return (&x)[i]; }
	float& operator[] (int i) { return (&x)[i]; }
	float length() const { return std::sqrt(x * x + y * y + z * z); }
};

inline NyaVec3 operator+(NyaVec3 b, NyaVec3 a) {
	return NyaVec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline NyaVec3 operator-(const NyaVec3& a, const NyaVec3& b) {
	return NyaVec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline NyaVec3 operator*(float a, const NyaVec3& b) {
	return NyaVec3(a * b.x, a * b.y, a * b.z);
}

inline NyaVec3 operator/(const NyaVec3& a, float b) {
	return NyaVec3(a.x / b, a.y / b, a.z / b);
}