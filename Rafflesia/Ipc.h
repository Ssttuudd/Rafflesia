#pragma once

#include <qthread.h>
#include <Windows.h>
#include <string>
#include <stdint.h>

#include "Game/Network/PacketHandler.h"

class PacketHandler;
class GameLogic;

class Ipc : public QThread, public INetworkInterface
{
	Q_OBJECT
public:
	~Ipc();

	bool connect(DWORD pid);

private:
	DWORD pid = 0;
	HANDLE pipeReceive;
	HANDLE pipeSend;
	BYTE* buffer;

	bool receivedHello = false;

protected:
	void run();
	void sendPacket(const char* data, int len) override;
	void sendShutdown();

signals:
	void sendMessage(QString);
	void onPacketData(const std::vector<char>&, bool);
};

