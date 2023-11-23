#pragma once
#include <iostream>
#include <windows.h>

typedef uint64_t(*func)(...);

namespace globals {
	extern HMODULE h_kernelbase;
	extern HMODULE h_starrailbase;
	extern HMODULE h_unityplayer;
	extern HMODULE h_starrail;
	extern HMODULE h_user32;
	extern HMODULE h_module;

	extern func create_hook;
	extern func setup_vm;
	extern func loadlibraryexw;
	extern func showwindow;
}