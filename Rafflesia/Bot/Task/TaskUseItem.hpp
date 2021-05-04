#pragma once

#include "Task.h"

class TaskUseItem : public Task
{
public:
	TaskUseItem(uint32_t _itemId) {
		itemId = _itemId;
	}

	bool update(float dt, Game& game, Bot& bot) override {
		timeSinceUse += dt;
		if (timeSinceUse >= cooldown
			&& isValid(game, bot, player->getTarget())
			&& bot.requestUseSkill(itemId)) {
			timeSinceUse = 0;
		}
		return false;
	}

private:
	float cooldown = 1;
	float timeSinceUse = 0;
	uint32_t itemId;
};

