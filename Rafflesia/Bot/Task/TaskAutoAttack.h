#pragma once

#include <vector>

#include "Task.h"
#include "Game/LocalPlayer.h"

class TaskAutoAttack: public Task
{
public:
	bool update( float dt, Game& game, Bot& bot ) override;
};

