#pragma once

#include "GameData.h"

struct ActorData {
	uint32_t id = 0;
	Position position;
};

class Actor
{
protected:
	uint32_t id = 0;
	EEntity type = EEntity::UNK;

	Position position = { 0 };

public:
	bool isNpc() const { return type == EEntity::NPC; }
	bool isPlayer() const { return type == EEntity::PLAYER; }
	bool isItem() const { return type == EEntity::ITEM; }
	bool isCharacter() const { return type == EEntity::PLAYER || type == EEntity::NPC; }
	uint32_t getId() const { return id; }
	void setId(uint32_t _id) { id = _id; }
	Position getPosition() const { return position; }
	double getDistance(const Position& targetPos) const;
	double getDistanceFromActor(const Actor* otherActor) const;

	void setPosition(uint32_t x, uint32_t y, uint32_t z);
	void setPosition(const Position& position);
	void setHeading(uint32_t heading) { position.heading = heading; }

	// Used to specify no entity
	static const uint32_t ENTITY_NONE = -1;

	bool operator==(const Actor& rhs);
};

