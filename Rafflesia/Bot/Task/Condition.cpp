#include "Condition.h"

namespace Condition {
	ConditionFunc createIsAlive(bool expectedResult) {
		return [expectedResult](Game& game, Bot& bot, void* data) {
			const auto res = data && !((Character*)data)->isDead();
			return expectedResult == res;
		};
	}

	ConditionFunc createIsAttackable(bool expectedResult) {
		return [expectedResult](Game& game, Bot& bot, void* data) {
			const auto res = data && ((Character*)data)->isAttackable() && !((Character*)data)->isDead();
			return expectedResult == res;
		};
	}

	ConditionFunc createIsNpc(bool expectedResult) {
		return [expectedResult](Game& game, Bot& bot, void* data) {
			const auto res = data && ((Character*)data)->isNpc();
			return expectedResult == res;
		};
	}

	ConditionFunc createCharacterStat(EStat stat, EComparison comparison, EConditionTarget target, int value, bool expectedResult) {
		return [stat, comparison, target, value, expectedResult](Game& game, Bot& bot, void* data) {

			bool res = false;
			if (Character* character = (Character*)data) {
				if (target == EConditionTarget::TARGET) {
					if (!character) {
						return false;
					}
					character = character->getTarget();
				}

				const int refValue = stat == EStat::HP ? character->getHp() : character->getMp();
				res = (comparison == EComparison::ABOVE) && refValue >= value
					|| (comparison == EComparison::BELLOW) && refValue < value;
			}

			return expectedResult == res;
		};
	}

	ConditionFunc createZDistance(int maxDistance, bool expectedResult) {
		return [maxDistance, expectedResult](Game& game, Bot& bot, void* data) {
			Actor* character = (Actor*)data;
			if (!character) {
				return false;
			}
			const auto res = distance(game.getPlayer()->getPosition().z, character->getPosition().z) < maxDistance;
			return expectedResult == res;
		};
	}

	ConditionFunc createDistance(int maxDistance, bool expectedResult) {
		return [maxDistance, expectedResult](Game& game, Bot& bot, void* data) {
			Actor* character = (Actor*)data;
			if (!character) {
				return false;
			}
			const auto res = distance(game.getPlayer()->getPosition(), character->getPosition()) < maxDistance;
			return expectedResult == res;
		};
	}

	ConditionFunc createIsInPolygon(bool expectedResult) {
		return [expectedResult](Game& game, Bot& bot, void* data) {
			Actor* character = (Actor*)data;
			if (!character) {
				return false;
			}
			const auto res = bot.isInPolygons(character->getPosition());
			return expectedResult == res;
		};
	}

	ConditionFunc createIsFiltered(bool expectedResult) {
		return [expectedResult](Game& game, Bot& bot, void* data) {
			Character* character = (Character*)data;
			bool res = bot.isObjectFiltered(character->getId());
			if (character->isNpc()) {
				res |= bot.isFamilyFiltered((Npc*)character);
			}
			return expectedResult == res;
		};
	}

	ConditionFunc createNpcId(int npcId, bool expectedResult) {
		return [npcId, expectedResult](Game& game, Bot& bot, void* data) {
			const auto res = ((Npc*)data)->GetNpcId() == npcId;
			return expectedResult == res;
		};
	}

	ConditionFunc createIsSpoiled(bool expectedResult) {
		return [expectedResult](Game& game, Bot& bot, void* data) {
			return expectedResult == ((Character*)data)->isSpoiled();
		};
	}

	ConditionFunc createPlayerHasBuff(int buffId, bool expectedResult) {
		return [buffId, expectedResult](Game& game, Bot& bot, void* data) {
			return expectedResult == game.hasBuff(buffId);
		};
	}

	ConditionFunc createHasName(const std::string& name) {
		return [name](Game& game, Bot& bot, void* data) {
			const auto targetName = ((Character*)data)->getName();
			return name == targetName;
		};
	}

}