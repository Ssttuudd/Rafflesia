#pragma once

#include <stdint.h>

#include "GameData.h"

enum class EStatus
{
	LEVEL = 0x01,
	EXP = 0x02,
	STR = 0x03,
	DEX = 0x04,
	CON = 0x05,
	INT = 0x06,
	WIT = 0x07,
	MEN = 0x08,

	CUR_HP = 0x09,
	MAX_HP = 0x0A,
	CUR_MP = 0x0B,
	MAX_MP = 0x0C,

	P_ATK = 0x11,
	ATK_SPD = 0x12,
	P_DEF = 0x13,
	EVASION = 0x14,
	ACCURACY = 0x15,
	CRITACAL = 0x16,
	M_ATK = 0x17,
	CAST_SPD = 0x18,
	M_DEF = 0x19,
	PVP_FLAG = 0x1A,
	REPUTATION = 0x1B,

	CUR_CP = 0x21,
	MAX_CP = 0x22
};

struct Status
{
	Status(uint8_t type, uint32_t value);
	uint8_t type = 0;
	uint32_t value = 0;
};