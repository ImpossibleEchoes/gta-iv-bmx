// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include "bmx_patch.h"
#include <stdio.h>
#include "addrs.h"

void enableConsole() {
	FILE* pFile = NULL;
	AllocConsole();
	SetConsoleTitleA("cringe2");
	freopen_s(&pFile, "CONOUT$", "w", stdout);
	freopen_s(&pFile, "CONOUT$", "w", stderr);
	freopen_s(&pFile, "CONIN$", "r", stdin);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//enableConsole();
		//system("pause");
		initAddrsDynamicCE();
		bmx_patch::patch();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

