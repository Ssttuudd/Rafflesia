#pragma once

#include <optional>
#include <unordered_set>
#include <vector>

#include "Module.h"

#include <iostream>
#include <fstream>

enum class ELevelTick {
	LEVELTICK_TimeOnly = 0,
	LEVELTICK_ViewportsOnly = 1,
	LEVELTICK_All = 2,
	LEVELTICK_PauseTick = 3,
};

class Ipc;
class Game
{
public:
	void init(const Module& engine, Ipc* ipc);
	void sendPacket(const char* buffer);

private:
	Ipc* ipc;

	bool refreshItems = false;
	bool refreshNpc = true;

	typedef DWORD(_stdcall* SendClientPacket)(DWORD, const char*, ...);

	SendClientPacket sendClientPacket;

	float waitingX = 0, waitingY = 0, waitingZ = 0;
};

