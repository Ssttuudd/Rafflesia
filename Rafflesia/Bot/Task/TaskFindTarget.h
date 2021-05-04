#pragma once

#include <vector>

#include "Task.h"

class TaskFindTarget : public Task {
public:
	void enter(Game& game, Bot& bot) override;
	bool update(float dt, Game& game, Bot& bot) override;

private:
	Character* foundTarget = nullptr;

	float timeSinceTargetAttempt = 0;
	std::vector<int> ignoreTargets;
	bool sentRequest = false;
};

class TaskBotFindTarget : public Task {
public:
	bool update(float dt, Game& game, Bot& bot) override;

private:
	Character* foundTarget = nullptr;
};