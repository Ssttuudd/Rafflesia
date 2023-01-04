#pragma once
#include <Windows.h>
#include <qstring.h>

#define INJECTOR_EXE_PATH L"ThreadHijacking.exe"
#define ROOT_DLL_PATH L"Root.dll"
#define AAD_DLL_PATH L"AAD.dll"
#define PPID_SPOOFED L"svchost.exe"

bool isDllLoadedInProcess( DWORD pid, const wchar_t* dllName );

bool injectAAD( int targetProcess );
bool injectRoot( int targetProcess );
DWORD deleguateInjection( int targetProcess, const std::wstring& dllName );
void checkFiles();
