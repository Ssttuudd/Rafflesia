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
	Task::update(dt, game, bot);

	if( !player || player->isDead() )
		return false;

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

	auto distance = player->getDistance( targetLocation );
	if( distance < stopDistance )
		return true;

	if (sentPacket) {
		timeSinceLastmove += dt;

		if (timeSinceLastmove > moveCooldown) {
			timeSinceLastmove = 0;
			moveToTarget(bot);
		}

	}
	else {
		targetLocation = target->getPosition();
		moveToTarget(bot);
		sentPacket = true;
	}

	return false;
}

void TaskMoveToTarget::moveToTarget(Bot& bot) {
	bot.moveToLocation( targetLocation );
}