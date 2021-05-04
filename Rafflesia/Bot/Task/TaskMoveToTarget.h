#pragma once

#include "Task.h"

class TaskMoveToTarget : public Task {
public:
	TaskMoveToTarget(int stopDistance);
	void enter(Game& game, Bot& bot) override;
	bool update(float dt, Game& game, Bot& bot) override;

private:
	int stopDistance = 0;
	bool sentPacket = false;
	Position targetLocation;
	float timeSinceLastmove = 0;
	float moveCooldown = 1;
	bool targetAggroFirst = false;

	void moveToTarget(Bot& bot);
};