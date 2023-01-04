#pragma once

#include "Character.h"
#include "GameData.h"

struct NpcData : ActorData {
	uint32_t npcId{ 0 };
	bool attackable{ false };
	int32_t heading{ 0 };
};

class Npc : public Character
{
private:
	uint32_t npcId = 0;

public:
	Npc(NpcData data);
	void SetHP(uint32_t HPMax, uint32_t HPCur);

	uint32_t GetNpcId() const { return npcId; }
};