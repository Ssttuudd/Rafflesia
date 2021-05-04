#pragma once

#include <vector>

#include "Task.h"
#include "Game/LocalPlayer.h"

class TaskAutoAttack: public Task
{
public:
	bool TaskAutoAttack::update(float dt, Game& game, Bot& bot) {
		auto player = game.getPlayer();
		auto target = player->getTarget();

		if (!target || target->isDead()) {
			player->setState(EState::STAND);
			return true;
		}

		if (isValid(game, bot, game.getPlayer()->getTarget())) {
			if (!player->isAttacking()) {
				bot.requestAttack(target);
			}
		}

		Task::update(dt, game, bot);

		return false;
	}
};

