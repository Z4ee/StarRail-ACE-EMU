#include "main.h"

BOOL hk_showwindow(HWND hWnd, int nCmdShow) {
	return true;
}

HMODULE hk_loadlibraryexw(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) try {
	auto ret = reinterpret_cast<HMODULE>(globals::loadlibraryexw(lpLibFileName, hFile, dwFlags));
	if (lpLibFileName && ret && lstrlenW(lpLibFileName) == 16 && lstrcmpW(lpLibFileName, L"GameAssembly.dll") == 0) {
		auto encoded_base = new char[0x20];
		*reinterpret_cast<uint64_t*>(encoded_base + 0x10) = (reinterpret_cast<uint64_t>(ret) - 0x30271D25137D50CFll) ^ 0x787761445AC0321Dll;

		auto h_res = FindResourceA(globals::h_module, MAKEINTRESOURCEA(BINARY), MAKEINTRESOURCEA(GLOBAL));

		if (!h_res)
			throw std::runtime_error("unable to find resource");

		auto h_data = LoadResource(globals::h_module, h_res);

		if (!h_data)
			throw std::runtime_error("unable to load resource");

		auto data = reinterpret_cast<uint8_t*>(LockResource(h_data));
		globals::setup_vm(encoded_base, 0x9EB4000, 0x6B18, data, data + 0x5E43);
		delete[] encoded_base;
		FreeResource(h_data);
	}
	return ret;
}
catch (std::runtime_error& err) {
	MessageBoxA(0, err.what(), "Fatal error", 0);
	TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) try {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		if (!globals::h_unityplayer)
			throw std::runtime_error("unable to load unityplayer.dll");

		globals::h_module = hModule;
		globals::create_hook(&globals::loadlibraryexw, hk_loadlibraryexw);
		globals::create_hook(&globals::showwindow, hk_showwindow);

		auto addr = reinterpret_cast<unsigned char*>(reinterpret_cast<uint64_t>(globals::h_starrail) + 0xA6B3E); // starrail.exe entry point
		*reinterpret_cast<DWORD*>(addr + 0x6) = 0;
		*reinterpret_cast<FARPROC*>(addr + 0xA) = GetProcAddress(globals::h_unityplayer, "UnityMain");

		DWORD old_protect = 0;

		VirtualProtect((PCHAR)globals::h_unityplayer + 0x1560E0A, 8, PAGE_EXECUTE_READWRITE, &old_protect);

		// *reinterpret_cast<DWORD64*>((PCHAR)globals::h_unityplayer + 0x1560E0A) = 0x5250C3C3C3C3C3C3;

	}
	return TRUE;
}
catch (std::runtime_error& err) {
	MessageBoxA(0, err.what(), "Fatal error", 0);
	TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
	return 0;
}
