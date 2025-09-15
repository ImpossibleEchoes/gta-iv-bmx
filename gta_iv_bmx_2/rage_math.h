#pragma once
#include <cmath>

#define RAGE_PI 3.1415926535f

__forceinline float smoothDamp(float current_value, float previous_smoothed_value, float delta_time, float smooth_time) {
	if (current_value == previous_smoothed_value)
		return current_value;
	if (smooth_time <= 0.0001f || delta_time <= 0.0f)
		return current_value;
	float alpha = 1.0f - expf(-delta_time / smooth_time);
	return alpha * current_value + (1.0f - alpha) * previous_smoothed_value;
}

__forceinline float normalizeAngle(float angle) {
	angle = fmodf(angle, 2.0f * RAGE_PI);
	if (angle > RAGE_PI)
		angle -= 2.0f * RAGE_PI;
	else if (angle < -RAGE_PI)
		angle += 2.0f * RAGE_PI;
	return angle;
}

__forceinline float smoothDampAngle(float target_angle, float current_angle, float delta_time, float smooth_time) {
	if (current_angle == target_angle)
		return current_angle;
	else if (smooth_time <= 0.0001f || delta_time <= 0.0f)
		return target_angle;

	float angle_diff = normalizeAngle(target_angle - current_angle);

	float alpha = 1.0f - expf(-delta_time / smooth_time);
	float smoothed_diff = alpha * angle_diff;

	//return current_angle + smoothed_diff;
	return normalizeAngle(current_angle + smoothed_diff);
}

struct Vector3 {
	float x, y, z, w;
	
	void normalize();
	void safeNormalize();

	__forceinline Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }

	__forceinline Vector3& operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	__forceinline Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	__forceinline Vector3& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	__forceinline Vector3 operator/(float scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	__forceinline Vector3& operator/=(float scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	__forceinline float getLengthSq() const {
		return (x * x) + (y * y) + (z * z);
	}

	__forceinline Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	__forceinline Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	__forceinline float getLength() const { return sqrtf(x * x + y * y + z * z); }

	__forceinline Vector3* cross(Vector3 &result, Vector3& other) const {
		result.x = (y * other.z) - (z * other.y);
		result.y = (z * other.x) - (x * other.z);
		result.z = (x * other.y) - (y * other.x);
		return &result;
	}

	__forceinline float distance(const Vector3& other) const {
		float deltaX = x - other.x;
		float deltaY = y - other.y;
		float deltaZ = z - other.z;

		float distanceSquared = (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ);

		return sqrtf(distanceSquared);
	}

	__forceinline float dot(const Vector3& other) const {
		return (x * other.x) + (y * other.y) + (z * other.z);
	}
};

struct Vector4 {
	float x, y, z, w;
};

struct Vec3 {
	float x, y, z;

	__forceinline float getLength() { return sqrtf(x * x + y * y + z * z); }
};

struct Quaternion {
	float x, y, z, w;

	void normalize();

	Quaternion operator*(const Quaternion& other) const;

	// Функция для создания кватерниона из оси и угла
	// axis: Вектор, представляющий ось вращения (должен быть нормализован)
	// angle_rad: Угол вращения в радианах
	void fromAxisAngle(const Vector3& axis, float angle_rad);
};

struct Matrix34 {
	Vector3 a, b, c, d;

	void identity();

	void identityRot();

	void fromQuaternion(const Quaternion& quat);

	void setYRot(float rot);

	void mult3x3(Matrix34* pMtx);
	void mult(Matrix34* pMtx);

	__forceinline float getZAngle() const { return atan2f(-a.y, a.x); }

	void normalizeOrthonormalBasis();

	Matrix34* multiplySubtractTranslation(Matrix34* a2, Matrix34* a3);

	Vector3* transformVector(Vector3& result, Vector3& v) const {
		result.x = (a.x * v.x) + (b.x * v.y) + (c.x * v.z);
		result.y = (a.y * v.x) + (b.y * v.y) + (c.y * v.z);
		result.z = (a.z * v.x) + (b.z * v.y) + (c.z * v.z);
		return &result;
	}
};