#pragma once

#include <functional>
#include <map>
#include <vector>

#include "Game/Game.h"
#include "PacketStream.h"
#include "Shared/Protocol.h"

class INetworkInterface {
public:
	virtual void sendPacket(const char* data, int len) = 0;
};

class PacketHandler
{
public:
	PacketHandler(Game& game, INetworkInterface* networkInterface);

	void parse(const std::vector<char>& data, bool sent);

	void requestAction(EAction action);
	void requestPickup(const DroppedItem* item);
	void selectTarget(Character* target, const Position& playerPos);
	void moveToLocation(const Position& playerPos, const Position& targetPos);
	void useskill(uint32_t skillId);
	void useItem(uint32_t itemId);
	void answerPartyRequest(bool answer);
	void requestDropItem(uint32_t itemId, uint64_t quantity, const Position& position);
	void validatePosition(Character* character);

	void init306();
	void init140();

private:
	Game& game;
	INetworkInterface* networkInterface;
	std::map<EPacketSC, std::function<void(PacketIStream&, Game&)>> parseMapSC;
	std::map<EPacketCS, std::function<void(PacketIStream&, Game&)>> parseMapCS;
};

