#include "TaskMove.h"

TaskMove::TaskMove(int _stopDistance) {
	stopDistance = _stopDistance;
}

void TaskMove::enter(Game& game, Bot& bot) {
	Task::enter(game, bot);
	sentPacket = false;
	timeSinceLastmove = 0;
}

bool TaskMove::update(float dt, Game& game, Bot& bot) {
	/*if (!player->canMove()) {
		return false;
	}*/

	Task::update(dt, game, bot);

	auto target = game.getPlayer()->getTarget();
	if (!target) {
		return true;
	}

	auto aggro = game.getFirstPlayerAggro();
	if (aggro && aggro != target) {
		target = aggro;
		timeSinceLastmove = moveCooldown;
	}

	if (sentPacket) {
		timeSinceLastmove += dt;

		if (timeSinceLastmove > moveCooldown) {
			timeSinceLastmove = 0;
			targetLocation = target->getPosition();
			moveToTarget(bot);
		}

		auto distance = player->getDistance(targetLocation);
		return distance < stopDistance;
	}
	else {
		targetLocation = target->getPosition();
		moveToTarget(bot);
		sentPacket = true;
	}

	return false;
}

void TaskMove::moveToTarget(Bot& bot) {
	auto loc = targetLocation;
	loc.x += rand() % 10 - 5;
	loc.y += rand() % 10 - 5;
	bot.moveToLocation(loc);
}