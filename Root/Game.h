#pragma once

#include <optional>
#include <unordered_set>
#include <vector>

#include <iostream>
#include <fstream>

#include "Module.h"
#include "../Shared/IpcShared.h"

class Ipc;
class Game
{
public:
	void init(const Module& engine, Ipc* ipc);
	void sendPacket(const char* buffer);

	void setPlayerPosition( const FVector& position ) { playerPosition = position; }
	const FVector& getPlayerPosition() { return playerPosition; }

	void setPacketFormaterAddr( DWORD ptr ) { packetFormaterAddr = ptr; }

private:
	Ipc* ipc;

	bool refreshItems = false;
	bool refreshNpc = true;

	typedef DWORD(_stdcall* SendClientPacket)(DWORD, const char*, ...);

	SendClientPacket sendClientPacket;

	FVector playerPosition;
	DWORD packetFormaterAddr{ 0 };
};

