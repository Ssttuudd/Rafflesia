#pragma once

#include <memory>
#include <string>

#include "Actor.h"

class Character;
using CharacterPtr = std::shared_ptr<Character>;

class Character : public Actor
{
public:
	void init(CharacterPtr character);

	// Setters
	void setHp(uint32_t val) { hpCur = val; }
	void setMaxHp(uint32_t val) { hpMax = val; }
	void setMp(uint32_t val) { mpCur = val; }
	void setMaxMp(uint32_t val) { mpMax = val; }
	void setCp(uint32_t val) { cpCur = val; }
	void setMaxCp(uint32_t val) { cpMax = val; }
	void setState(EState _state) { state = _state; }
	void setLevel(uint16_t _level) { level = _level; }
	void setName(const std::string& _name) { name = _name; }
	void setAttackable(bool _attackable) { attackable = _attackable; }
	void setTarget(Character* target);
	void setInCombat(bool value) { inCombat = value; }
	void resetTarget() { target = nullptr; }
	void setSpeed(uint16_t runSpeed, uint16_t walkspeed, uint16_t swimRunSpeed, uint16_t swimWalkSpeed, double speedMultiplier);
	void setDead();
	void setDestination(int32_t x, int32_t y, int32_t z);
	void setSweepable(bool value) { sweepable = value; }
	void setSpoiled(bool value) { spoiled = value; }
	void setInParty(bool value) { inParty = value; }

	// Getters
	uint32_t getHp() const { return hpCur; }
	uint32_t getMaxHp() const { return hpMax; }
	uint32_t getHpPercent() const { return hpCur ? hpCur * 100 / hpMax : 0; }
	uint32_t getMp() const { return mpCur; }
	uint32_t getMaxMp() const { return mpMax; }
	uint32_t getMpPercent() const { return mpCur ? mpCur * 100 / mpMax : 0; }
	const std::string& getName() const { return name; }

	bool isSitting() const { return state == EState::SIT; }
	bool isStanding() const { return state != EState::SIT; }
	bool isAttacking() const { return state == EState::ATTACK; }
	bool canMove() const { return state != EState::SIT && state != EState::CAST; };
	void stopMove();

	Character* getTarget() const { return target; }
	uint16_t getLevel() const { return level; }
	uint32_t getCurrentSpeed() const;

	bool isInCombat() const { return inCombat; }
	bool isAttackable() const { return attackable; }
	bool isDead() const { return dead; }
	bool isAlive() const { return !dead; }
	bool isSweepable() const { return sweepable; }
	bool isSpoiled() const { return spoiled; }
	bool isRunning() const { return running; }
	bool isMoving() const { return moving; }
	bool isInParty() const { return inParty; }

	void update(float dt);

protected:
	bool moving = false;
	bool inCombat = false;
	bool dead = false;
	bool attackable = true;
	bool sweepable = false;
	bool running = false;
	bool spoiled = false;
	bool inParty = false;
	std::string name;

	uint32_t hpCur = 0;
	uint32_t hpMax = 0;
	uint32_t mpCur = 0;
	uint32_t mpMax = 0;
	uint32_t cpCur = 0;
	uint32_t cpMax = 0;

	uint16_t runSpeed;
	uint16_t walkSpeed;
	uint16_t swimRunSpeed;
	uint16_t swimWalkSpeed;
	uint16_t flyRunSpeed;
	uint16_t flyWalkSpeed;
	double speedMultiplier;

	uint16_t level = 0;
	EState state = EState::STAND;

	Character* target = nullptr;

	Direction moveDirection;
	Position moveDestination;
	uint16_t moveCount = 0;
	float heading = 0;
	float timeSinceLastMove = 0;
	const float moveUpdateRate = 0.1f;
};

