#pragma once

#include <memory>

#include "Bot/Bot.h"
#include "Game/Character.h"
#include "Game/Game.h"
#include "Game/MathUtils.h"
#include "Game/Npc.h"



class Bot;
namespace Condition {

	enum class EStat {
		HP,
		MP
	};

	enum class EComparison {
		ABOVE,
		BELLOW
	};

	enum class EConditionTarget {
		SELF,
		TARGET
	};

	using ConditionFunc = std::function<bool(Game&, Bot&, void*)>;

	ConditionFunc createIsAlive(bool expectedResult);
	ConditionFunc createIsAttackable(bool expectedResult);
	ConditionFunc createIsNpc(bool expectedResult);
	ConditionFunc createCharacterStat(EStat stat, EComparison comparison, EConditionTarget target, int value, bool expectedResult);
	ConditionFunc createZDistance(int maxDistance, bool expectedResult);
	ConditionFunc createDistance(int maxDistance, bool expectedResult);
	ConditionFunc createIsInPolygon(bool expectedResult);
	ConditionFunc createIsFiltered(bool expectedResult);
	ConditionFunc createNpcId(int npcId, bool expectedResult);
	ConditionFunc createIsSpoiled(bool expectedResult);
	ConditionFunc createPlayerHasBuff(int buffId, bool expectedResult);
	ConditionFunc createHasName(const std::string& name);
}