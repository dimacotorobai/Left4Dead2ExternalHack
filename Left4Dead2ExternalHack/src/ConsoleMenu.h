#pragma once

struct ModuleOption
{
	std::string sModuleName;
	uintptr_t uModuleAddress;
};

struct MenuOption
{
	std::string sMenuOptionName;
	uintptr_t uMenuOptionKey;
	bool bMenuOptionStatus;
	void (*funcMenuFunctionOn)();
	void (*funcMenuFunctionOff)();
};

class ConsoleMenu
{
public:
	ConsoleMenu(const wchar_t *szWindowName, const wchar_t *szProcessName, DWORD dwProcessID);
	~ConsoleMenu();

	void PrintMenu();
	void Run();

	void AddModule(std::string sModuleName, uintptr_t uModuleBaseAddress);
	void AddMenu(std::string sMenuOption, uintptr_t uVirtualKey, void(*funcOn)(), void(*funcOff)());


private:
	wchar_t szTitle[32];
	wchar_t szProcessName[32];
	DWORD dwProcessID;

	std::vector<ModuleOption> ModuleList;
	std::vector<MenuOption> MenuList;


	bool bUpdateMenu = false;
};

