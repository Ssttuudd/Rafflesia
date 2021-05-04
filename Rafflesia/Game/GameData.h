#pragma once

#include <cmath>
#include <stdint.h>

enum class EProtocol {
	VERSION_28 = 28,
	VERSION_306 = 306
};

enum class EEntity {
	UNK = 0,
	NPC = 1,
	PLAYER = 2,
	ITEM = 3,
};

enum class EState {
	SIT = 0,
	STAND = 1,
	CAST = 2,
	ATTACK = 3,
};

enum class EAction {
	SIT_STAND = 0,
	WALK_RUN = 1,
	ATTACK = 2,
	EXCHANGE = 3,
	NEXT_TARGET = 4,
	PICKUP = 5,
	ASSIST = 6,
	INVITE = 7
};

enum class ESystemMessage {
	INVALID_TARGET = 0x006d,
	DISTANCE_TOO_FAR = 0x02ec,
	CANNOT_SEE_TARGET = 0x00b5,
	ALREADY_SPOILED = 0x0165,
	SPOIL_ACTIVATED = 0x0264,
	NONE = 0xffff
};

struct Position
{
	int32_t x = 0;
	int32_t y = 0;
	int32_t z = 0;
	int32_t heading = 0;

	bool operator==(const Position& p) const {
		return x == p.x && y == p.y && z == p.z;
	}

	bool operator==(const Position& p) {
		return x == p.x && y == p.y && z == p.z;
	}

	bool operator!=(const Position& p) {
		return !(x == p.x && y == p.y && z == p.z);
	}

	bool operator!=(const Position& p) const {
		return !(x == p.x && y == p.y && z == p.z);
	}

	int32_t getX() const { return x; };
	int32_t getY() const { return y; };
	int32_t getZ() const { return z; };
	int32_t getH() const { return heading; };
};

struct Direction
{
	float x = 0;
	float y = 0;

	bool operator==(const Direction& p) {
		return x == p.x && y == p.y;
	}

	bool operator!=(const Direction& p) {
		return !(x == p.x && y == p.y);
	}

	float length() const {
		return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
	}

	void normalize() {
		const auto l = length();
		x /= l;
		y /= l;
	}
};

struct Skill {
	uint32_t id;
	uint16_t level;
	uint16_t sublevel;
	uint32_t passive;
	uint32_t reuseDelayGroup;
	bool disabled;
	bool enchanted;
};

struct Buff {
	uint32_t id;
	uint32_t level;
	uint32_t timeLeft;
};