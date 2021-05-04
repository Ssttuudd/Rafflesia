#pragma once

#include "Windows.h"

#include <functional>

std::string hexStr(unsigned char* data, int len);

class Game;
class Ipc
{
public:
	Ipc();
	~Ipc();
	void init(Game* game);
	void update();
	void sendStart();
	void sendIpc(const char* data, size_t len);
	void sendTcp(const char* buffer);
	void sendNpcInfo(int32_t id, uint32_t npcId, bool isAttackable, int32_t x, int32_t y, int32_t z);
	void sendLocalPlayerInfo(int32_t id, uint32_t npcId, bool isAttackable, int32_t x, int32_t y, int32_t z, std::wstring name);
	bool shouldShutdown();

private:
	HANDLE pipeReceive;
	HANDLE pipeSend;
	BYTE* buffer;
	bool receivedShutdown = false;
	Game* game;
};

