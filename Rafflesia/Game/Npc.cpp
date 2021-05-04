#include "Npc.h"

#include <windows.h>

#include "Shared/SimpleBinStream.hpp"

// If this layout changes, the refresh packet generation in the dll must change as well.
Npc::Npc(NpcData data) {
	id = data.id;
	attackable = data.attackable;
	position = data.position;
	npcId = data.npcId;

	this->type = EEntity::NPC;
}

void Npc::SetHP(uint32_t HPMax, uint32_t HPCur)
{
	this->hpMax = HPMax;
	this->hpCur = HPCur;
}
