#pragma once

namespace hackprocess
{
	//
	//External Functions
	//

	/*Retrieves the Process ID of a Window, must provide the Window Name*/
	DWORD GetWindowProcessIdEx(const wchar_t *szWindowName);

	/*Retrieves the Process ID of a Process, must provide the Process Name*/
	DWORD GetProcessIdEx(const wchar_t *szProcessName);

	/*Retrieves the Module Base Address, must provide the Module Name and the Process ID of the Process*/
	uintptr_t GetModuleBaseAddressEx(const wchar_t *szModuleName, DWORD dwProcessID);

	/*Calculates the Dynamic Address, must provide static address plus the offsets*/
	uintptr_t FindDynamicAddressEx(HANDLE hProcess, uintptr_t uBaseAddress, std::vector<uintptr_t> Offsets);

	/*Patch Memory, must provide and address, size, and a buffer*/
	BOOL PatchMemoryEx(HANDLE hProcess, uintptr_t uAddress, const BYTE * src, size_t N);


	//
	//Internal Functions
	//

	/*Retrieves the Process ID of the current process*/
	DWORD GetProcessId();

	/*Retrieves the Module Base Address from the calling process*/
	uintptr_t GetModuleBaseAddress(const wchar_t *szModuleName);

	/*Calculates the Dynamic Address*/
	uintptr_t FindDynamicAddress(uintptr_t uBaseAddress, std::vector<uintptr_t>Offsets);

	/*Patches Memory within the process*/
	BOOL PatchMemory(uintptr_t uAddress, const BYTE* src, size_t N);


}