#include "pch.h"
#include "Process.h"
#include "hackprocess.h"


Process::Process(): dwProcessID(0), hProcess(0)
{
}

Process::~Process()
{
}

BOOL Process::GetProcessID(const wchar_t *szProcessName)
{
	if (dwProcessID = hackprocess::GetProcessIdEx(szProcessName))
	{
		wmemset(this->szProcessName, L'\0', 32);
		wmemcpy(this->szProcessName, szProcessName, 32);
		return TRUE;
	}
	return FALSE;
}

BOOL Process::GetDeviceContext(const wchar_t * szWindowName)
{
	hWnd = FindWindow(nullptr, szWindowName);
	if (hWnd)
	{
		hdc = GetDC(hWnd);
		if (hdc)
			return TRUE;
	}
	return FALSE;
}

uintptr_t Process::GetModuleBaseAddress(const wchar_t *szModuleName)
{
	return hackprocess::GetModuleBaseAddressEx(szModuleName, dwProcessID);
}

uintptr_t Process::FindDynamicAddress(uintptr_t uAddress, std::vector<uintptr_t> Offsets)
{
	if (hProcess)
		return hackprocess::FindDynamicAddressEx(hProcess, uAddress, Offsets);
	return 0;
}

BOOL Process::PatchMemory(uintptr_t uAddress, BYTE * shellcode, size_t N)
{
	return hackprocess::PatchMemoryEx(hProcess, uAddress, shellcode, N);
}

BOOL Process::Attach()
{
	if (hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessID))
		return (BOOL)hProcess;
	return FALSE;
}

BOOL Process::Close()
{
	if (hProcess)
		return CloseHandle(hProcess);
	return FALSE;
}
