#include "TaskMoveToTarget.h"

TaskMoveToTarget::TaskMoveToTarget(int _stopDistance) :
	timeSinceLastmove{ 0 }
{
	stopDistance = _stopDistance;
}

void TaskMoveToTarget::enter(Game& game, Bot& bot) {
	Task::enter(game, bot);
	sentPacket = false;
	timeSinceLastmove = 0;
	targetAggroFirst = bot.getSettings()->targetAggroFirst;
}

bool TaskMoveToTarget::update(float dt, Game& game, Bot& bot) {
	/*if (!player->canMove()) {
		return false;
	}*/

	Task::update(dt, game, bot);

	auto target = game.getPlayer()->getTarget();
	if (!target) {
		return true;
	}

	targetLocation = target->getPosition();
	if (targetAggroFirst) {
		auto aggro = game.getFirstPlayerAggro();
		if (aggro && aggro != target) {
			target = aggro;
			bot.requestSelectTarget(target);
			timeSinceLastmove = moveCooldown;
		}
	}

	if (sentPacket) {
		timeSinceLastmove += dt;

		if (timeSinceLastmove > moveCooldown) {
			timeSinceLastmove = 0;
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

void TaskMoveToTarget::moveToTarget(Bot& bot) {
	auto loc = targetLocation;
	loc.x += rand() % 10 - 5;
	loc.y += rand() % 10 - 5;
	bot.moveToLocation(loc);
}