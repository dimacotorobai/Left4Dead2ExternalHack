#pragma once
#include "../src/pch.h"
#include "structures.h"
#include "offsets.h"

//Constants
#define PI 3.14159265
HBRUSH EnemyBrush = CreateSolidBrush(0x000000FF);

//Process Stuff
Process pProcess;
uintptr_t clientDll;
uintptr_t engineDll;

//Local Player Stuff
DWORD dwLocalPlayer;
DWORD dwLocalPlayerFlags;
DWORD dwForceJump;
DWORD dwEntityList;
DWORD dwLocalPlayerBone;
uint8_t FlagsValue;

//Default Screen
float screenX = 1176;
float screenY = 664;

bool bAutoShoot = false;

//Returns the distance between player and entity
inline float PythagTheoream(vector3_t vec3PlayerPos, vector3_t vec3EntityPos)
{
	return sqrt(pow(vec3EntityPos.x - vec3PlayerPos.x, 2) + pow(vec3EntityPos.y - vec3PlayerPos.y, 2) + pow(vec3EntityPos.z - vec3PlayerPos.z, 2));

}
inline float GetYaw(vector3_t vec3PlayerPos, vector3_t vec3EntityPos)
{
	float x_relative = vec3EntityPos.x - vec3PlayerPos.x;
	float y_relative = vec3EntityPos.y - vec3PlayerPos.y;

	return atan2(y_relative, x_relative) * 180 / PI;

}
inline float GetPitch(vector3_t vec3PlayerPos, vector3_t vec3EntityPos, float Hypotenuse)
{
	return -asin(((vec3EntityPos.z - vec3PlayerPos.z) / Hypotenuse)) * 180 / PI;
}

inline void DrawFilledRect(int x, int y, int w, int h)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(pProcess.hdc, &rect, EnemyBrush);
}
inline void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	DrawFilledRect(x, y, w, thickness);
	DrawFilledRect(x, y, thickness, h);
	DrawFilledRect((x + w), y, thickness, h);
	DrawFilledRect(x, y + h, w + thickness, thickness);
}

vector3_t WorldToScreen(vector3_t pos, float matrix[4][4])
{
	//Multiply matrix with vector
	float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];
	float _z = matrix[2][0] * pos.x + matrix[2][1] * pos.y + matrix[2][2] * pos.z + matrix[2][3];
	float _w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

	//Check if entity is on screen
	if (_w < 0.1f)
		return { 0.0f, 0.0f, 0.0f };

	//Normalized device coordinates
	vector3_t NDC;
	NDC.x = _x / _w;
	NDC.y = _y / _w;
	NDC.z = _z / _w;
	
	//Screen Position
	vector3_t Screen;
	Screen.x =  (screenX / 2 * NDC.x) + (NDC.x + screenX / 2);
	Screen.y = -(screenY / 2 * NDC.y) + (NDC.y + screenY / 2);

	return Screen;
}

void AimbotOn()
{
	//Get the view angles
	DWORD dwViewAngles = pProcess.ReadMemory<DWORD>(engineDll + 0x004258EC) + 0x4AAC;
	vector3_t vec3LocalPAngles = pProcess.ReadMemory<vector3_t>(dwViewAngles);

	//Get local player, localplayer pos, and localplayer camera height
	dwLocalPlayer = pProcess.ReadMemory<DWORD>(clientDll + 0x0072B144);
	vector3_t lpPos = pProcess.ReadMemory<vector3_t>(dwLocalPlayer + 0x124);
	vector3_t lpPosH = pProcess.ReadMemory<vector3_t>(dwLocalPlayer + 0xF4);
	lpPos = { lpPos.x + lpPosH.x, lpPos.y + lpPosH.y , lpPos.z + lpPosH.z };

	//Set max float in order to determine the closest enemy(first entity becomes closest)
	float distance = FLT_MAX;
	float pitch = FLT_MAX;
	float yaw = FLT_MAX;

	//GetEntity
	for (int i = 3; i < 500; i++)
	{
		if (DWORD dwTeamate = pProcess.ReadMemory<DWORD>(clientDll + 0x0072B154 + i * 0x10))
		{
			//Get Entity Team and Status
			int team = pProcess.ReadMemory<int>(dwTeamate + 0xE4);
			int alive = pProcess.ReadMemory<int>(dwTeamate + 0x144);

			if ((team == 3) && (alive != 0))
			{
				DWORD dwModelNameAddr = pProcess.ReadMemory<DWORD>(dwTeamate + 0x60);

				//Read Model Name to determine if charger
				struct ModelName
				{
					char szName[8];
				} modelName;

				modelName = pProcess.ReadMemory<ModelName>(dwModelNameAddr + 0x14);
				modelName.szName[7] = '\0';

				//If charger head is at 16 instead of 14
				int j = 14;
				if (!strcmp("charger", modelName.szName))
					j = 16;

				//Get feet location
				vector3_t vec3FeetPos = pProcess.ReadMemory<vector3_t>(dwTeamate + 0x124);

				//Get bone matrix
				DWORD dwTeamatebonematrix_tAddr = pProcess.ReadMemory<DWORD>(dwTeamate + 0x06C0);
				bonematrix_t matbonematrix_t = pProcess.ReadMemory<bonematrix_t>(dwTeamatebonematrix_tAddr + (j * 0x30));

				//Convert bone matrix to vector3_t
				vector3_t vec3TeamPos = { matbonematrix_t.x, matbonematrix_t.y, matbonematrix_t.z };

				DWORD dwViewMatrix = pProcess.ReadMemory<DWORD>(engineDll + 0x00426AA8) + 0x9C;
				matrix4x4_t viewMat = pProcess.ReadMemory<matrix4x4_t>(dwViewMatrix);

				vector3_t headScreen = WorldToScreen(vec3TeamPos, viewMat.mat);
				vector3_t feetScreen = WorldToScreen(vec3FeetPos, viewMat.mat);

				int height = feetScreen.y - headScreen.y;
				int width = height / 2;
				DrawBorderBox(headScreen.x - width / 2, headScreen.y, width, height, 3);


				//World to Screen Conversion
				RECT rect;
				GetClientRect(pProcess.hWnd, &rect);
				screenX = rect.right - rect.left;
				screenY = rect.bottom - rect.top;
				std::cout << "(" << screenX << ", " << screenY << ")" << std::endl;

				//Get information between localplayer and entity
				float _distance = PythagTheoream(lpPos, vec3TeamPos);
				float _pitch = GetPitch(lpPos, vec3TeamPos, _distance);
				float _yaw = GetYaw(lpPos, vec3TeamPos);

				//Local Yaw copy {0, 360} instead of {-180, 180} for angle difference check
				float _yaw_enemy = (_yaw < 0) ? _yaw + 360 : _yaw;
				float _yaw_local = (vec3LocalPAngles.y < 0) ? vec3LocalPAngles.y + 360 : vec3LocalPAngles.y;

				//Set maximum rotation angle so aimbot is not crazy doing really bit rotations
				if (abs(_pitch - vec3LocalPAngles.x) < 30 && abs(_yaw_local - _yaw_enemy) < 30)
				{
					if (_distance < distance)
					{
						distance = _distance;
						pitch = _pitch;
						yaw = _yaw;
					}
				}
			}
		}
	}

	if (pitch < 360)
	{
		//Write to angles
		vector3_t vec3ViewAngles = { pitch, yaw, 0.0f };

		//Activate with ALT
		if (GetAsyncKeyState(VK_MENU))
		{
			pProcess.WriteMemory<vector3_t>(dwViewAngles, vec3ViewAngles);
			bAutoShoot = true;
		}
	}
}
void EspOn()
{
	for (int i = 0; i < 3; i++)
	{
		if (DWORD dwTeamate = pProcess.ReadMemory<DWORD>(clientDll + 0x0072B154 + i * 0x10))
		{
			vector3_t vec3Feet = pProcess.ReadMemory<vector3_t>(dwTeamate + 0x124);

			//Get bone matrix
			vector3_t vec3View = pProcess.ReadMemory<vector3_t>(dwTeamate + 0xF4);
			vector3_t vec3Head = { vec3Feet.x, vec3Feet.y, +vec3Feet.z + vec3View.z};

			//Convert bone matrix to vector3_t
			//vector3_t vec3Head = { vec3Feet.x, vec3Feet.y, vec3Feet.z + 60.0f };

			matrix4x4_t viewMat = pProcess.ReadMemory<matrix4x4_t>(0x084497E4);

			vector3_t headScreen = WorldToScreen(vec3Head, viewMat.mat);
			vector3_t feetScreen = WorldToScreen(vec3Feet, viewMat.mat);

			int height = feetScreen.y - headScreen.y;
			int width = height / 2;
			DrawBorderBox(headScreen.x - width / 2, headScreen.y, width, height, 3);
		}
	}


}
void AutoShootOn()
{
	if (bAutoShoot)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(1);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		bAutoShoot = false;
		Sleep(1);
	}

}
void BunnyHopOn()
{
	dwLocalPlayer = pProcess.ReadMemory<DWORD>(clientDll + 0x0071A5D8);
	dwLocalPlayerFlags = dwLocalPlayer + 0xF0;

	if (GetAsyncKeyState(VK_SPACE))
	{
		FlagsValue = pProcess.ReadMemory<uint8_t>(dwLocalPlayerFlags);
		if (FlagsValue == 129)
		{
			/*Clear the dwForceJump to Normal*/
			pProcess.WriteMemory<int>(clientDll + 0x74AA48, 4);
			Sleep(1);

			/*Set dwForceJump or simulate space key press*/
			pProcess.WriteMemory<int>(clientDll + 0x74AA48, 5);
			Sleep(1);

			/*Clear dwForceJump or simulate space key press*/
			pProcess.WriteMemory<int>(clientDll + 0x74AA48, 4);
		}
	}
}
void Exit()
{
	pProcess.Close();
	exit(0);
}


