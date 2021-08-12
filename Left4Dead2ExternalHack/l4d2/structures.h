#pragma once

struct vector3_t
{
	float x;
	float y;
	float z;
};
struct matrix4x4_t
{
	float mat[4][4];
};
struct bonematrix_t
{
	float pad0[3];
	float x;
	float pad1[3];
	float y;
	float pad2[3];
	float z;
};
