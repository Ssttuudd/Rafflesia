#pragma once

#include <qthread.h>
#include <Windows.h>
#include <string>
#include <stdint.h>

#include "Game/Network/PacketHandler.h"

class PacketHandler;
class GameLogic;
class Position;

class Ipc : public QThread, public INetworkInterface
{
	Q_OBJECT
public:
	~Ipc();

	bool connect(DWORD pid);
	void stop();

private:
	DWORD pid{ 0 };
	HANDLE pipeReceive{ nullptr };
	HANDLE pipeSend{ nullptr };
	HANDLE threadHandle{ nullptr };
	BYTE* buffer{ nullptr };;
	bool shouldStop{ false };

	bool receivedHello = false;

protected:
	void run();
	void sendPacket(const char* data, int len) override;
	void sendShutdown();

signals:
	void sendMessage(QString);
	void onPacketData( const std::vector<char>&, bool );
	void onGameInfos(const Position& playerPosition);
	void onRootConnected();
};

