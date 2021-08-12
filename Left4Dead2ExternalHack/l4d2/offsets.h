#pragma once

namespace l4d2
{
	namespace client
	{
		//Self
		DWORD flFovOffset = 0x7B3204;
		DWORD iForceJumpOffset = 0x74AA48; // 4 Reset, 5 Jump

		uintptr_t dwLocalPlayerOffset = 0x0072B144; // Start entity goes by up by 0x10
		uintptr_t dwLocalTeamate1Offset = 0x0072B154;
		uintptr_t dwLocalTeamate2Offset = 0x0072B144;
		uintptr_t dwLocalTeamate3Offset = 0x0072B144;

		DWORD m_dwEntityListId = 0x0058;
		DWORD m_szModelName = 0x0060;
		DWORD m_iPosX = 0x0070;
		DWORD m_iPosY = 0x0074;
		DWORD m_iPosZ = 0x0078;

		DWORD m_vec3PlayerVelocity = 0x0088;
		DWORD m_vec3PlayerPosition = 0x0094;
		DWORD m_vec3PlayerViewAngles = 0x00B8;

		DWORD m_iTeamNum = 0xE4; // Infected = 3, Survivors = 2
		DWORD m_iHealth = 0xEC;
		DWORD m_iFlags = 0xF0; // Regular = 129, Jump = 130, Crouch = 131
		DWORD m_vec3ViewOffsets = 0xF4; // Z determines the relative height of the camera

		DWORD m_fIsAlive = 0x144;	//0 = Dead,~0 = Alive
		DWORD m_iIsAlive2 = 0x160;	//0 = Dead, 1 = Alive

		DWORD m_matBoneMatrix = 0x06C0;
	}
	
	namespace engine
	{
		uintptr_t m_vec3ViewAngles = 0x004258EC;
	}
}
