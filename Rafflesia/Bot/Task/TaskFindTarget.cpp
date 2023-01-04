#include "TaskFindTarget.h"

#include <limits>

#include "Bot/Bot.h"
#include "Game/MathUtils.h"

void TaskFindTarget::enter(Game& game, Bot& bot) {
	Task::enter(game, bot);

	sentRequest = false;
	foundTarget = nullptr;
	ignoreTargets.clear();
}

bool TaskFindTarget::update(float dt, Game& game, Bot& bot) {
	if (!player || player->isDead()) {
		return false;
	}
	if (foundTarget) {
		if (sentRequest) {
			timeSinceTargetAttempt += dt;
			// if can't target // TODO REWORK THAT
			if (timeSinceTargetAttempt > 5) {
				ignoreTargets.push_back(foundTarget->getId());
				foundTarget = nullptr;
			}
			// wait for the server answer
			return foundTarget == game.getPlayer()->getTarget();
		}
		else {
			timeSinceTargetAttempt = 0;
			return bot.requestSelectTarget(foundTarget);
		}
	}

	foundTarget = game.getFirstPlayerAggro();
	if (foundTarget) {
		return false;
	}

	float lastDistance = (std::numeric_limits<float>::max)();
	auto characters = game.getCharacters();
	for (const auto charEntry : characters) {
		const auto character = charEntry.second;
		/*if (character->getId() != player->getId())*/ {
			if (isValid(game, bot, character.get())) {
				auto dist = distance(character->getPosition(), player->getPosition());
				if (dist < lastDistance) {
					if (std::find(ignoreTargets.begin(), ignoreTargets.end(), character->getId()) == std::end(ignoreTargets)) {
						lastDistance = dist;
						foundTarget = character.get();
					}
				}
			}
		}
	}

	Task::update(dt, game, bot);

	return false;
}

bool TaskBotFindTarget::update(float dt, Game& game, Bot& bot) {
	
	if (bot.findTarget()) {
		return true;
	}

	Task::update(dt, game, bot);

	return false;
}