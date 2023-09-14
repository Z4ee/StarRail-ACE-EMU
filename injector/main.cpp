#include <Windows.h>
#include <iostream>
#include <vector>
#include "inject.h"

using namespace std;

vector<unsigned char> load_file(const string& filename) {
	ifstream file(filename, ios::binary | ios::ate);

	if (!file.is_open())
		throw runtime_error("unable to open file");

	auto size = file.tellg();
	file.seekg(0, ios::beg);

	vector<unsigned char> data(size);

	if (!file.read(reinterpret_cast<char*>(data.data()), size))
		throw runtime_error("unable to read file");

	file.close();

	return data;
}

int main() try {
	auto data = load_file("star-rail-ace.dll");
	char szFile[MAX_PATH] = { 0 };
	DWORD bytesRead = GetPrivateProfileStringA("Paths", "StarRailPath", "", szFile, MAX_PATH, "sr-injector.ini");
	if (bytesRead == 0) {
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Executable Files\0StarRail.exe\0All Files (*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (!GetOpenFileNameA(&ofn))
			throw std::runtime_error("unable to get starrail.exe path");

		WritePrivateProfileStringA("Paths", "StarRailPath", szFile, "sr-injector.ini");
	}
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcessA(NULL, szFile, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
		throw std::runtime_error("unable to launch starrail.exe");
	
	if (!inject::ManualMapDll(pi.hProcess, data.data(), data.size()))
		throw std::runtime_error("unable to inject dll");

	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}
catch (std::runtime_error& err) {
	MessageBoxA(0, err.what(), "Fatal error", 0);
	return -1;
}
