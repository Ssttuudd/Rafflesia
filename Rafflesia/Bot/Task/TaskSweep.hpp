#pragma once

#include "Task.h"

class TaskSweep : public Task
{
public:
	bool update(float dt, Game& game, Bot& bot) override {
		auto target = player->getTarget();
		if (target && target->isSweepable()) {
			bot.requestUseSkill(42);
			return false;
		}
		return true;
	}
};

