#pragma once
#include "structures.h"
#include "offsets.h"

#define PI 3.14159265359



/*Returns the distance between player and entity*/
float PythagTheoream(Vector3 vec3PlayerPos, Vector3 vec3EntityPos)
{
	return sqrt(pow(vec3EntityPos.x - vec3PlayerPos.x, 2) + pow(vec3EntityPos.x - vec3PlayerPos.x, 2) + pow(vec3EntityPos.x - vec3PlayerPos.x, 2));

}

/*Get the Yaw between two entities*/
float GetYaw(Vector3 vec3PlayerPos, Vector3 vec3EntityPos)
{
	float x_relative = vec3EntityPos.x - vec3PlayerPos.x;
	float y_relative = vec3EntityPos.y - vec3PlayerPos.y;

	return atan2(y_relative, x_relative) * 180 / PI;

}

/*Get the Pitch between two entities*/
float GetPitch(Vector3 vec3PlayerPos, Vector3 vec3EntityPos, float Hypotenuse)
{
	return -asin(((vec3EntityPos.z - vec3PlayerPos.z) / Hypotenuse)) * 180 / PI;
}
