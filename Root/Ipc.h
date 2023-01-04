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
	void sendGameInfos();
	bool shouldShutdown();

private:
	void connect();

	HANDLE pipeReceive;
	HANDLE pipeSend;
	BYTE* buffer;
	bool needShutdown = false;
	Game* game;
};

