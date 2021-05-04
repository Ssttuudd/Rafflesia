#pragma once

#include "Task.h"

class TaskMove : public Task {
public:
	TaskMove(int stopDistance);
	void enter(Game& game, Bot& bot) override;
	bool update(float dt, Game& game, Bot& bot) override;

private:
	int stopDistance;
	bool sentPacket = false;
	Position targetLocation;
	float timeSinceLastmove;
	float moveCooldown = 1;

	void moveToTarget(Bot& bot);
};