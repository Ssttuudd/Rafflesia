#pragma once

#include <vector>

#include "Task.h"

class TaskUseSkill: public Task
{
public:
	TaskUseSkill(uint32_t _skillId, float _cooldown) {
		skillId = _skillId;
		cooldown = _cooldown;
	}

	void enter(Game& game, Bot& bot) {
		Task::enter(game, bot);
		timeSinceUse = cooldown;
	}

	bool update(float dt, Game& game, Bot& bot) {
		Task::update(dt, game, bot);
		timeSinceUse += dt;
		if (timeSinceUse >= cooldown
			&& isValid(game, bot, player->getTarget())
			&& bot.requestUseSkill(skillId)) {
			timeSinceUse = 0;
		}
		return false;
	}

private:
	float cooldown = 1;
	float timeSinceUse = 0;
	uint32_t skillId;
};

