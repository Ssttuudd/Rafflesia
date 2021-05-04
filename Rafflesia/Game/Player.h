#pragma once

#include <Windows.h>

#include "Character.h"
#include "GameData.h"

struct PlayerData: public Actor {
	int32_t id;
	Position position;
	std::string name;
	std::string title;
};

struct UserInfoData {
	int32_t id;
	std::string name;
	uint8_t isGm;
	uint8_t race;
	uint8_t sex;
	int32_t classId;
	int32_t classId2;
	uint8_t level;
	uint32_t maxHp;
	uint32_t maxMp;
	uint32_t maxCp;
	uint32_t curHp;
	uint32_t curMp;
	uint32_t curCp;
	uint32_t sp;
	uint32_t xp;
	bool weaponEquiped = true;
	uint32_t patk;
	uint32_t pdef;
	uint32_t mdef;
	uint32_t matk;
	uint32_t pAccuracy;
	uint32_t mAccuracy;
	uint32_t pEvasion;
	uint32_t mEvasion;
	uint32_t pCritical;
	uint32_t mCritical;
	uint32_t atkSpd;
	uint32_t castSpd;
	uint32_t pvpCount;
	uint32_t karma;
	uint32_t posX;
	uint32_t posY;
	uint32_t posZ; 
	uint16_t runSpeed;
	uint16_t walkSpeed;
	uint16_t swimRunSpeed;
	uint16_t swimWalkSpeed;
	uint16_t flyRunSpeed;
	uint16_t flyWalkSpeed;
	double speedMultiplier;
};

class Player : public Character
{
private:

public:
	Player();
	Player(PlayerData data);
	Player(UserInfoData data);
};
