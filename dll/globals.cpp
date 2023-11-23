#include "globals.h"

namespace globals {
	HMODULE h_kernelbase = LoadLibraryA("kernelbase.dll");
	HMODULE h_starrailbase = LoadLibraryA("starrailbase.dll");
	HMODULE h_unityplayer = LoadLibraryA("unityplayer.dll");
	HMODULE h_user32 = LoadLibraryA("user32.dll");
	HMODULE h_starrail = GetModuleHandleA(NULL);
	HMODULE h_module = 0;

	func create_hook = reinterpret_cast<func>(reinterpret_cast<uint64_t>(h_starrailbase) + 0x1CAA20);
	func setup_vm = reinterpret_cast<func>(reinterpret_cast<uint64_t>(h_starrailbase) + 0xE3EE0);
	func loadlibraryexw = reinterpret_cast<func>(GetProcAddress(h_kernelbase, "LoadLibraryExW"));
	func showwindow = reinterpret_cast<func>(GetProcAddress(h_user32, "ShowWindow"));
}