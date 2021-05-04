#pragma once

#include <memory>

#include "Player.h"

class LocalPlayer: public Player
{
public:
	LocalPlayer(const PlayerData& data);
};

using LocalPlayerPtr = std::shared_ptr<LocalPlayer>;

