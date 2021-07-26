#include "pch.h"
#include "Process.h"
#include "ConsoleMenu.h"

#define PI 3.14159265
#define EnemyPen 0x000000FF
HBRUSH EnemyBrush = CreateSolidBrush(0x000000FF);

Process pProcess;
uintptr_t clientDll;
uintptr_t engineDll;


DWORD dwLocalPlayer;
DWORD dwLocalPlayerHealth;
DWORD dwLocalPlayerFlags;
DWORD dwForceJump;
DWORD dwEntityList;
DWORD dwLocalPlayerBone;
uint8_t FlagsValue;

float screenX = 1920;
float screenY = 1080;

bool bAutoShoot = false;

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

//Returns the distance between player and entity
float PythagTheoream(vector3_t vec3PlayerPos, vector3_t vec3EntityPos)
{
	return sqrt(pow(vec3EntityPos.x - vec3PlayerPos.x, 2) + pow(vec3EntityPos.y - vec3PlayerPos.y, 2) + pow(vec3EntityPos.z - vec3PlayerPos.z, 2));

}
float GetYaw(vector3_t vec3PlayerPos, vector3_t vec3EntityPos)
{
	float x_relative = vec3EntityPos.x - vec3PlayerPos.x;
	float y_relative = vec3EntityPos.y - vec3PlayerPos.y;

	return atan2(y_relative, x_relative) * 180/PI;

}
float GetPitch(vector3_t vec3PlayerPos, vector3_t vec3EntityPos, float Hypotenuse)
{
	return -asin(((vec3EntityPos.z - vec3PlayerPos.z) / Hypotenuse)) * 180/PI;
}

unsigned int j = 1;

void DrawFilledRect(int x, int y, int w, int h)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(pProcess.hdc, &rect, EnemyBrush);
}
void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	DrawFilledRect(x, y, w, thickness);
	DrawFilledRect(x, y, thickness, h);
	DrawFilledRect((x + w), y, thickness, h);
	DrawFilledRect(x, y + h, w + thickness, thickness);
}
void DrawLine(float startx, float starty, float endx, float endy)
{
	int a, b = 0;
	HPEN hOpen;
	HPEN hNPen = CreatePen(PS_SOLID, 2, EnemyPen);
	hOpen = (HPEN)SelectObject(pProcess.hdc, hNPen);
	MoveToEx(pProcess.hdc, startx, starty, nullptr);
	a = LineTo(pProcess.hdc, endx, endy);
	DeleteObject(SelectObject(pProcess.hdc, hOpen));
}
vector3_t WorldToScreen(vector3_t pos, matrix4x4_t matrix)
{
	float _x = matrix.mat[0][0] * pos.x + matrix.mat[0][1] * pos.y + matrix.mat[0][2] * pos.z + matrix.mat[0][3];
	float _y = matrix.mat[1][0] * pos.x + matrix.mat[1][1] * pos.y + matrix.mat[1][2] * pos.z + matrix.mat[1][3];
	float w = matrix.mat[3][0] * pos.x + matrix.mat[3][1] * pos.y + matrix.mat[3][2] * pos.z + matrix.mat[3][3];

	float inv_w = 1.0f / w;
	_x *= inv_w;
	_y *= inv_w;

	float x = screenX * 0.5f;
	float y = screenY * 0.5f;

	x += 0.5f*_x*screenX + 0.5f;
	x += 0.5f*_y*screenY + 0.5f;

	return { x, y, w };
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
	for (int i = 3; i < 1500; i++)
	{

		//DrawBorderBox(100, 100, 100, 200, 2);


		if (DWORD dwTeamate = pProcess.ReadMemory<DWORD>(clientDll + 0x0072B154 + i * 0x10))
		{
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
				if (!strcmp("charger", modelName.szName))
					j = 16;
				else
					j = 14;

				//Get bone matrix
				DWORD dwTeamatebonematrix_tAddr = pProcess.ReadMemory<DWORD>(dwTeamate + 0x06C0);
				bonematrix_t matbonematrix_t = pProcess.ReadMemory<bonematrix_t>(dwTeamatebonematrix_tAddr + (j * 0x30));

				//Convert bone matrix to vector3_t
				vector3_t vec3TeamPos = { matbonematrix_t.x, matbonematrix_t.y, matbonematrix_t.z };

				//World to Screen Conversion
				RECT rect;
				GetWindowRect(pProcess.hWnd, &rect);
				screenX = rect.right - rect.left;
				screenY = rect.bottom - rect.top;
				
				//matrix4x4_t viewMatrix = pProcess.ReadMemory<matrix4x4_t>(0x07D8FAE4);
				//vector3_t w2s = WorldToScreen(vec3TeamPos, viewMatrix);
				//DrawBorderBox(w2s.x, w2s.y, 10, 10, 2);
				
			

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
		//Console Info
		//std::cout << "Distance: " << distance << std::endl;
		//std::cout << "Pitch: " << pitch << std::endl;
		//std::cout << "Yaw: " << yaw << std::endl;
		//std::cout << "J: " << std::dec << j << std::endl;

		//Write to angles
		vector3_t vec3ViewAngles = { pitch, yaw, 0.0f };

		//Activate with ALT
		if (GetAsyncKeyState(VK_MENU))
		{
			pProcess.WriteMemory<vector3_t>(dwViewAngles, vec3ViewAngles);
			bAutoShoot = true;
		}
	}
	if (GetAsyncKeyState(VK_F9))
		j--;
	if (GetAsyncKeyState(VK_F10))
		j++;


	//Sleep(1);
}
void EspOn()
{



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
void GetEntityNames()
{
	dwEntityList = clientDll + 0x0072B144;
	DWORD dwEntity = 0;
	int iEntityListId = 0;

	unsigned char szName[48];


	for (int i = 0; i < 350; i++)
	{
		dwEntity = 0;
		dwEntity = pProcess.ReadMemory<DWORD>(dwEntityList + 0x10 * i);
		if (dwEntity)
		{
			iEntityListId = pProcess.ReadMemory<int>(dwEntity + 0x0058);
			DWORD dwModelNameAddr = pProcess.ReadMemory<DWORD>(dwEntity + 0x60);
			if (dwModelNameAddr)
			{
				memset(szName, '\0', sizeof(szName));
				for (int j = 0; j < 48; j++)
				{
					szName[j] = pProcess.ReadMemory<char>(dwModelNameAddr + 4 + j * 1);
					if (szName[j] == '\0')
						break;
				}

				char buffer[16];
				memset(buffer, '\0', 16);
				memcpy(buffer, szName, 15);

				if (!strcmp("models/infected", buffer))
					std::cout << "i: " << std::hex << i * 0x10 << std::dec << ", Id: " << iEntityListId << ", szName: " << szName << std::endl;
			}
		}

	}
}
void BunnyHopOn()
{
	dwLocalPlayer = pProcess.ReadMemory<DWORD>(clientDll + 0x0071A5D8);
	dwLocalPlayerFlags = dwLocalPlayer + 0xF0;
	dwLocalPlayerHealth = dwLocalPlayer + 0xEC;

	if (GetAsyncKeyState(VK_SPACE))
	{
		FlagsValue = pProcess.ReadMemory<uint8_t>(dwLocalPlayerFlags);
		if (FlagsValue == 129)
		{
			/*Clear the dwForceJump to Normal*/
			pProcess.WriteMemory<int>(clientDll + 0x74AA48, 4);
			Sleep(1);

			/*Set dwForceJump or simulate space key press*/
			//keybd_event(VK_SPACE, 0x39, KEYEVENTF_EXTENDEDKEY | 0, 0);
			pProcess.WriteMemory<int>(clientDll + 0x74AA48, 5);
			Sleep(1);

			/*Clear dwForceJump or simulate space key press*/
			//keybd_event(VK_SPACE, 0x39, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			pProcess.WriteMemory<int>(clientDll + 0x74AA48, 4);

		}
	}

	//Sleep(1);
}
void Exit()
{
	pProcess.Close();
	exit(0);
}



int main(int argc, char **argv)
{
	SetConsoleTitle(L"Left 4 Dead 2 External Trainer");

	pProcess;
	if (pProcess.GetProcessID(L"left4dead2.exe"))
	{
		if (clientDll = pProcess.GetModuleBaseAddress(L"client.dll"))
		{
			BOOL Status = pProcess.GetDeviceContext(L"Left 4 Dead 2 - Direct3D 9");
			engineDll = pProcess.GetModuleBaseAddress(L"engine.dll");
			pProcess.Attach();

			ConsoleMenu console = ConsoleMenu(L"Left 4 Dead 2 External Trainer", L"left4dead2.exe", pProcess.dwProcessID);
			console.AddModule("client.dll", clientDll);
			console.AddModule("engine.dll", engineDll);

			console.AddMenu("[F1] Aimbot", VK_F1, &AimbotOn, nullptr);
			console.AddMenu("[F2] Auto Shoot", VK_F2, &AutoShootOn, nullptr);
			console.AddMenu("[F3] Bunny Hop", VK_F3, &BunnyHopOn, nullptr);
			console.AddMenu("[F4] Exit", VK_F4, &Exit, nullptr);

			console.Run();
		}
	}



	std::cin.get();
	return 0;
}