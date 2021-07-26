#include "pch.h"
#include "ConsoleMenu.h"


ConsoleMenu::ConsoleMenu(const wchar_t *szWindowName, const wchar_t *szProcessName, DWORD dwProcessID)
{
	wmemset(this->szTitle, L'\0', 32);
	wmemcpy(this->szTitle, szWindowName, 32);

	wmemset(this->szProcessName, L'\0', 32);
	wmemcpy(this->szProcessName, szProcessName, 32);

	this->dwProcessID = dwProcessID;

	SetConsoleTitle(this->szTitle);
}


ConsoleMenu::~ConsoleMenu()
{
}

void ConsoleMenu::PrintMenu()
{
	system("cls");

	std::wcout << "------------------ " << szTitle<< " ------------------" << std::endl;
	std::wcout << "| Process Name \t: " << szProcessName << std::endl;
	std::cout  << "| Process ID   \t: " << std::hex << dwProcessID << std::endl;

	for (int i = 0; i < ModuleList.size(); i++)
	{
		std::cout << "| " << ModuleList[i].sModuleName << " \t: 0x" << ModuleList[i].uModuleAddress << std::endl;
	}
	for (int i = 0; i <= wcslen(szTitle) + 2*strlen("------------------ ") - 1; i++)
		std::cout << "-";
	
	std::cout << std::endl << std::endl;

	for (int i = 0; i < MenuList.size(); i++)
	{
		std::cout << MenuList[i].sMenuOptionName;
		if (MenuList[i].bMenuOptionStatus)
		{
			std::cout << " < ON > " << std::endl;
		}
		else
		{
			std::cout << " < OFF > " << std::endl;
		}
	}
}

void ConsoleMenu::Run()
{
	while (true)
	{
		for (int i = 0; i < MenuList.size(); i++)
		{
			if (GetAsyncKeyState(MenuList[i].uMenuOptionKey))
			{
				MenuList[i].bMenuOptionStatus = !MenuList[i].bMenuOptionStatus;
				bUpdateMenu = true;
			}

			if (MenuList[i].bMenuOptionStatus && MenuList[i].funcMenuFunctionOn != nullptr)
			{
				MenuList[i].funcMenuFunctionOn();
			}
			else if(!MenuList[i].bMenuOptionStatus && MenuList[i].funcMenuFunctionOff != nullptr)
			{
				MenuList[i].funcMenuFunctionOff();
			}
		}


		if (bUpdateMenu)
		{
			PrintMenu();
			bUpdateMenu = false;
		}

		Sleep(10);
	}
}

void ConsoleMenu::AddModule(std::string sModuleName, uintptr_t uModuleBaseAddress)
{
	ModuleList.push_back({ sModuleName, uModuleBaseAddress });
	bUpdateMenu = true;
}

void ConsoleMenu::AddMenu(std::string sMenuOption, uintptr_t uVirtualKey, void (*funcOn)(), void (*funcOff)())
{
	MenuList.push_back({ sMenuOption, uVirtualKey, false, funcOn, funcOff});
	bUpdateMenu = true;
}
