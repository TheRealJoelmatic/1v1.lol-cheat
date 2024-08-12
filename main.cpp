#include "includes.h"
#include "ext/logger/logger.h"
#include "render/menu.h"
#include "sdk/sdk.h"
#include "render/hooks.h"
#include "ext/IL2CPP_Resolver/IL2CPP_Resolver.hpp"
#include "ext/IL2CPP_Resolver/lazyimporter.h"

#include <process.h>
#include "config/config.h"
#include "auth/key.h"

// For DLL mode
extern "C" __declspec(dllexport)

// Exporting function usable with SetWindowsHookEx
extern "C" __declspec(dllexport) int NextHook(int code, WPARAM wParam, LPARAM lParam) {
	return CallNextHookEx(NULL, code, wParam, lParam);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	if (!checkKey()) {
		MessageBoxA(nullptr, "No key found! \nPlease use the supplied loader", "Error", MB_OK);
		exit(0);
		return FALSE;
	}

	logger::init();
	logger::customize::setConsoleTitle("[-] Jm solultions +-+ 1V1.LOL Cheat [-]");

	logger::print("");
	logger::print("+----------------------------------+");
	logger::print("| [-] Jm solultions");
	logger::print("| [-] 1V1.LOL Cheat coded by: Joelmatic");
	logger::print("+----------------------------------+");
	logger::print("");
	logger::print("Loading SDK...");
	if (!sdk::init()) {
		logger::error("Failed to initialize SDK");
		MessageBox(nullptr, "Failed to initialize SDK", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return FALSE;
	}

	logger::print("");
	logger::print("+----------------------------------+");
	logger::print("");

	logger::print("Loading hooks...");
	if (!hooks::init()) {
		logger::error("Failed to initialize hooks");
		MessageBox(nullptr, "Failed to initialize hooks", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return FALSE;
	}
	logger::print(" --> Hooks Set");

	logger::print("");
	logger::print("+----------------------------------+");
	logger::print("");
	void* m_pMainThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());
	logger::print("Loading threads...");
	HANDLE playerThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)sdk::getplayers, NULL, NULL, NULL);
	if (playerThread == NULL) {
		logger::error("Failed to create player thread");
		MessageBox(nullptr, "Failed to create player thread", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return FALSE;
	}
	else {
		logger::print(" Player Thread Set");
	}
	logger::print(" --> Threads Set");


	logger::print("");
	logger::print("+----------------------------------+");
	logger::print("");
	logger::print("Loading config...");
	load_config("config1.cfg");
	logger::print("--> Loaded config");
	logger::print("");
	logger::print("+----------------------------------+");
	logger::print("");

	logger::print("Loading Render Hook...");

	if (!hooks::imgui()) {
		logger::error("Failed to initialize imgui");
		MessageBox(nullptr, "Failed to initialize imgui", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return FALSE;
	}

	while (TRUE) {
		Sleep(100000);
	}

	return TRUE;
}


static uintptr_t __cdecl I_beginthreadex(
	void* _Security,
	unsigned                 _StackSize,
	_beginthreadex_proc_type _StartAddress,
	void* _ArgList,
	unsigned                 _InitFlag,
	unsigned* _ThrdAddr) {

	return iat(_beginthreadex).get()(_Security, _StackSize, _StartAddress, _ArgList, _InitFlag, _ThrdAddr);
}

HMODULE _hMod;
BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		_hMod = hMod;
		I_beginthreadex(0, 0, (_beginthreadex_proc_type)MainThread, 0, 0, 0);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}