#pragma once

#include <windows.h>
#include <stdio.h>

class Console
{
public:
	Console() {
		AllocConsole();
		freopen_s(&m_file, "CONIN$", "r", stdin);
		freopen_s(&m_file, "CONOUT$", "w", stderr);
		freopen_s(&m_file, "CONOUT$", "w", stdout);
	}

	~Console() {
		FreeConsole();
	}

private:
	FILE* m_file;
};

