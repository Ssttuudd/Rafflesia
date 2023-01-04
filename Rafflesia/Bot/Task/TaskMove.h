#pragma once

#include "Task.h"

class TaskMove : public Task {
public:
	TaskMove(int stopDistance);
	void enter(Game& game, Bot& bot) override;
	bool update(float dt, Game& game, Bot& bot) override;

private:
	int stopDistance{ 0 };
	bool sentPacket{ false };
	Position targetLocation;
	float timeSinceLastmove{ 0.0f };
	float moveCooldown{ 1.0f };

	void moveToTarget(Bot& bot);
};