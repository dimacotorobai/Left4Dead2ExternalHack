#include "pch.h"
#include "hackprocess.h"

DWORD hackprocess::GetWindowProcessIdEx(const wchar_t *szWindowName)
{
	DWORD dwProcessID = 0;
	HWND hWindow = FindWindow(nullptr, szWindowName);
	if (hWindow)
	{
		GetWindowThreadProcessId(hWindow, &dwProcessID);
	}
	return dwProcessID;
}


DWORD hackprocess::GetProcessIdEx(const wchar_t *szProcessName)
{
	DWORD dwProcessID = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 ProcessEntry;
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

		//Enumarate through processes and find szProcessName
		if (Process32First(hSnapshot, &ProcessEntry))
		{
			do
			{
				if (!wcscmp(szProcessName, ProcessEntry.szExeFile))
				{
					dwProcessID = ProcessEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &ProcessEntry));
		}
	}

	//Close Handle and Return ProcessID
	CloseHandle(hSnapshot);
	return dwProcessID;
}

uintptr_t hackprocess::GetModuleBaseAddressEx(const wchar_t *szModuleName, DWORD dwProcessID)
{
	uintptr_t uModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcessID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry;
		ModuleEntry.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hSnapshot, &ModuleEntry))
		{
			do
			{
				if (!wcscmp(szModuleName, ModuleEntry.szModule))
				{
					uModuleBaseAddress = (uintptr_t)ModuleEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry));
		}
	}

	//Close handle and return module base address
	CloseHandle(hSnapshot);
	return uModuleBaseAddress;
}

uintptr_t hackprocess::FindDynamicAddressEx(HANDLE hProcess, uintptr_t uBaseAddress, std::vector<uintptr_t> Offsets)
{
	uintptr_t uDynamicAddress = uBaseAddress + Offsets[0];

	for (int i = 1; i < Offsets.size(); i++)
	{
		ReadProcessMemory(hProcess, (const void *)uDynamicAddress, &uDynamicAddress, sizeof(uDynamicAddress), nullptr);
		uDynamicAddress += Offsets[i];
	}

	return uDynamicAddress;
}

BOOL hackprocess::PatchMemoryEx(HANDLE hProcess, uintptr_t uAddress, const BYTE * src, size_t N)
{
	DWORD oldProtect;
	if (VirtualProtectEx(hProcess, (void *)uAddress, N, PAGE_EXECUTE_READWRITE, &oldProtect))
		if (WriteProcessMemory(hProcess, (void *)uAddress, src, N, 0))
			if (VirtualProtectEx(hProcess, (void *)uAddress, N, oldProtect, &oldProtect))
				return TRUE;
	return FALSE;
}

DWORD hackprocess::GetProcessId()
{
	return GetCurrentProcessId();
}

uintptr_t hackprocess::GetModuleBaseAddress(const wchar_t * szModuleName)
{
	return (uintptr_t)GetModuleHandle(szModuleName);
}

uintptr_t hackprocess::FindDynamicAddress(uintptr_t uBaseAddress, std::vector<uintptr_t> Offsets)
{
	uintptr_t uDynamicAddress = uBaseAddress + Offsets[0];
	for (int i = 1; i < Offsets.size(); i++)
	{
		uDynamicAddress = *(uintptr_t *)uDynamicAddress;
		uDynamicAddress += Offsets[i];
	}
	return uDynamicAddress;
}

BOOL hackprocess::PatchMemory(uintptr_t uAddress, const BYTE * src, size_t N)
{
	DWORD oldProtect;
	if (VirtualProtect((void*)uAddress, N, PAGE_EXECUTE_READWRITE, &oldProtect))
		if (memcpy((void*)uAddress, src, N))
			if (VirtualProtect((void*)uAddress, N, oldProtect, &oldProtect))
				return TRUE;
	return FALSE;
}

