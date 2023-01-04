#pragma once

#include <map>
#include <vector>

#include "Game/Game.h"
#include "Shared/Protocol.h"
#include "Packet.h"

class INetworkInterface {
public:
	virtual void sendPacket(const char* data, int len) = 0;
};

class PacketHandler
{
public:
	PacketHandler(Game& game, INetworkInterface* networkInterface);

	bool isPacketValid( const std::vector<char>& data ) const;
	APacket* getPacket( const std::vector<char>& data, bool clientToServer );

	void requestAction(EAction action);
	void requestPickup(const DroppedItem* item);
	void selectTarget(Character* target, const Position& playerPos);
	void moveToLocation(const Position& playerPos, const Position& targetPos);
	void useskill(uint32_t skillId);
	void useItem(uint32_t itemId);
	void answerPartyRequest(bool answer);
	void requestDropItem(uint32_t itemId, uint64_t quantity, const Position& position);
	void requestGameInfos();

private:
	Game& game;
	INetworkInterface* networkInterface;
	std::map<EPacketSC, APacket> parseMapSC;
	std::map<EPacketCS, APacket> parseMapCS;
};

