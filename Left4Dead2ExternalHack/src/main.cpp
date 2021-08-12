#include "pch.h"
#include "Process.h"
#include "ConsoleMenu.h"
#include "../l4d2/l4d2.h"

int main(int argc, char **argv)
{
	SetConsoleTitle(L"Left 4 Dead 2 External Trainer");

	pProcess;
	if (pProcess.GetProcessID(L"left4dead2.exe"))
	{
		if (pProcess.GetDeviceContext(L"Left 4 Dead 2 - Direct3D 9"))
		{
			//Attach to process
			pProcess.Attach();

			//Get modules base addresses
			clientDll = pProcess.GetModuleBaseAddress(L"client.dll");
			engineDll = pProcess.GetModuleBaseAddress(L"engine.dll");

			//Initialize Console Menu
			ConsoleMenu console = ConsoleMenu(L"Left 4 Dead 2 External Trainer", L"left4dead2.exe", pProcess.dwProcessID);
			console.AddModule("client.dll", clientDll);
			console.AddModule("engine.dll", engineDll);

			//Console Menu Options
			console.AddMenu("[F1] Aimbot", VK_F1, &AimbotOn, nullptr);
			console.AddMenu("[F2] Auto Shoot", VK_F2, &AutoShootOn, nullptr);
			console.AddMenu("[F3] Bunny Hop", VK_F3, &BunnyHopOn, nullptr);
			console.AddMenu("[F4] Exit", VK_F4, &Exit, nullptr);
			console.AddMenu("[F5] Esp", VK_F5, &EspOn, nullptr);

			//Run Console Menu
			console.Run();
		}
	}
	
	//Pause before closing
	std::cin.get();
	return 0;
}