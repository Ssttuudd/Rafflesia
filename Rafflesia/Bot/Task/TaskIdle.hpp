#pragma once

#include "Task.h"

class TaskIdle : public Task {
public:
	TaskIdle(int _minHp, int _maxHp) {
		minHp = _minHp;
		maxHp = _maxHp;
	}

	void enter(Game& game, Bot& bot) {
		Task::enter(game, bot);

	}

	bool update(float dt, Game& game, Bot& bot) {
		if (player->getHp() < minHp && !game.playerHasAggro()) {
			if (!player->isSitting()) {
				bot.requestSitStand();
			}
			return false;
		}
		else {
			if (player->isSitting()) {
				if (player->getHp() > maxHp || game.playerHasAggro()) {
					bot.requestSitStand();
					return false;
				}
				else {
					return false;
				}
			}
			else {
				return true;
			}
		}
	}

protected:
	int minHp;
	int maxHp;
};

