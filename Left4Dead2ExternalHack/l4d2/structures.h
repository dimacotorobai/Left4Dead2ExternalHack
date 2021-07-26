#pragma once

struct Vector3
{
	float x;
	float y;
	float z;

	/*
	Vector3()
		: x(0.0f), y(0.0f), z(0.0f) {}

	Vector3(float _x, float _y, float _z)
		: x(_x), y(_y), z(_z) {}

	Vector3 operator+(const Vector3& b)
	{
		Vector3 out = {x + b.x, y + b.y, z + b.z};
		return out;
	}
	Vector3 operator-(const Vector3& b)
	{
		Vector3 out = { x - b.x, y - b.y, z - b.z };
		return out;
	}
	*/

};

struct BoneMatrix
{
	float pad0[3];
	float x;
	float pad1[3];
	float y;
	float pad2[3];
	float z;
};

