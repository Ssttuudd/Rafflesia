#pragma once
#include <Windows.h>
#include <qstring.h>

#define EXE_FULL_PATH L"D:/Projects/Rafflesia/ThreadHijacking/Debug/ThreadHijacking.exe"
#define DLL_FULL_PATH L"D:/Projects/Rafflesia/Rafflesia/Debug/Root.dll"
#define PPID_SPOOFED L"svchost.exe"

DWORD deleguateInjection(int pid);

