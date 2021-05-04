#pragma once

#include "Task.h"
#include "Game/LocalPlayer.h"


class TaskPickup: public Task
{
public:
	TaskPickup(int _range, bool _sweep) {
		range = _range;
		sweep = _sweep;
	}
	bool update(float dt, Game& game, Bot& bot) {
		/*if (game.playerHasAggro()) {
			return true;
		}*/

		auto target = player->getTarget();
		auto playerPosition = player->getPosition();

		// Sweep first
		if (sweep && target && target->isSweepable()) {
			bot.requestUseSkill(42);
			return false;
		}

		// Then pickup
		// TODO handle "pick your on drop functionnality"
		bool pickingUp = false;
		auto& droppedItems = game.getItems();
		for (auto& i : droppedItems) {
			if (distance(i.second->getPosition(), playerPosition) < range) {
				bot.requestPickup(i.second.get());
				pickingUp = true;
				break;
			}
		}

		Task::update(dt, game, bot);

		return !pickingUp;
	}

private:
	int range = 0;
	bool sweep = false;
};

