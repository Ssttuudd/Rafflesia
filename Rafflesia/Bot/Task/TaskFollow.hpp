#pragma once

#include "Task.h"
#include "Game/LocalPlayer.h"
#include "Game/MathUtils.h"

#include <QDebug>
class TaskFollow : public Task
{
public:
	TaskFollow(float _stopDistance, float _startDistance) {
		stopDistance = _stopDistance;
		startDistance = _startDistance;
	}

	bool update(float dt, Game& game, Bot& bot) {
		if( !player || player->isDead() )
			return false;

		auto target = player->getTarget();
		if (target) {
			const auto dist = distance(target->getPosition(), player->getPosition());
			qDebug(QString::number(dist).toLatin1());
			if (moving) {
				if (dist < stopDistance) {
					bot.moveToLocation(player->getPosition());
					moving = false;
				}
			}
			else {
				if (dist > startDistance) {
					bot.moveToLocation(target->getPosition());
					moving = true;
				}
			}
		}

		Task::update(dt, game, bot);

		return false;
	}

private:
	float stopDistance;
	float startDistance;

	bool moving = false;
};