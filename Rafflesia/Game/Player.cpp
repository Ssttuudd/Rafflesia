#include "Player.h"

#include "Shared/SimpleBinStream.hpp"

Player::Player() {
	running = true;
}

Player::Player(PlayerData data)
{
	type = EEntity::PLAYER;
	id = data.id;
	name = data.name;
	position = data.position;
}

Player::Player(UserInfoData data)
{
	type = EEntity::PLAYER;
	id = data.id;
	name = data.name;
	setHp(data.curHp);
	setMaxHp(data.maxHp);
	setMp(data.curMp);
	setMaxMp(data.maxMp);
	setCp(data.curCp);
	setMaxCp(data.maxCp);
	setLevel(data.level);
	setName(data.name);
}