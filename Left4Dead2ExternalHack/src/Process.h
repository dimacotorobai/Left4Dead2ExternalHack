#pragma once
class Process
{
public:
	Process();
	~Process();

	BOOL GetProcessID(const wchar_t *szProcessName);
	BOOL GetDeviceContext(const wchar_t *szWindowName);
	uintptr_t GetModuleBaseAddress(const wchar_t *szModuleName);

	uintptr_t FindDynamicAddress(uintptr_t uAddress, std::vector<uintptr_t> Offsets);

	template<typename DataType>
	DataType ReadMemory(uintptr_t uAddress);

	template<typename DataType>
	BOOL WriteMemory(uintptr_t uAddress, DataType Value);

	BOOL PatchMemory(uintptr_t uAddress, BYTE *shellcode, size_t N);


	BOOL Attach();
	BOOL Close();

public:
	DWORD dwProcessID;
	wchar_t szProcessName[32];

public:
	HANDLE hProcess;
	HDC hdc;
	HWND hWnd;
};


//Template Specifications

template<typename DataType>
inline DataType Process::ReadMemory(uintptr_t uAddress)
{
	DataType dataBuffer;

	ReadProcessMemory(hProcess, (const void *)uAddress, &dataBuffer, sizeof(dataBuffer), nullptr);

	return dataBuffer;
}

template<typename DataType>
inline BOOL Process::WriteMemory(uintptr_t uAddress, DataType Value)
{
	return WriteProcessMemory(hProcess, (void *)uAddress, &Value, sizeof(Value), nullptr);
}
