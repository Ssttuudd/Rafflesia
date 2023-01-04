#pragma once

#include <memory>

#include "Player.h"

class LocalPlayer: public Player
{
public:
	LocalPlayer(const PlayerData& data);

	const Position& getClientPosition() { return clientPosition; }
	void setClientPosition( const Position& position ) { clientPosition = position; }

private:
	Position clientPosition;
};

using LocalPlayerPtr = std::shared_ptr<LocalPlayer>;

