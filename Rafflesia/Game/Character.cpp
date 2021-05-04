#include "Character.h"

#include "Game/MathUtils.h"

void Character::init(CharacterPtr character) {
	hpMax = character->getMaxHp();
	hpCur = character->getHp();
	mpMax = character->getMaxMp();
	mpCur = character->getMp();
}

void Character::setTarget(Character* _target)
{
	target = _target;
}

void Character::setDead()
{
	hpCur = 0;
	dead = true;
}

#include <QDebug>
#include <QString>
void Character::setDestination(int32_t x, int32_t y, int32_t z)
{
	timeSinceLastMove = 0;
	moving = true;
	moveDestination = { x, y, z };
	moveDirection = { (float)moveDestination.x - position.x, (float)moveDestination.y - position.y };
	heading = atan2(moveDirection.y, moveDirection.x) * 180.f / 3.14159f;
	moveCount = std::ceil(moveDirection.length() / (getCurrentSpeed() * moveUpdateRate));
	moveDirection.normalize();
}

void Character::update(float dt) {
	if (moveCount > 0) {
		timeSinceLastMove += dt;
		if (timeSinceLastMove > moveUpdateRate) {
			timeSinceLastMove -= moveUpdateRate;
			const auto speed = getCurrentSpeed() * moveUpdateRate;
			position.x += std::floor(moveDirection.x * speed);
			position.y += std::floor(moveDirection.y * speed);
			moveCount--;
		}
	}
	else if(isMoving()) {
		position = moveDestination;
		moving = false;
	}
}

void Character::setSpeed(uint16_t _runSpeed, uint16_t _walkspeed, uint16_t _swimRunSpeed, uint16_t _swimWalkSpeed, double _speedMultiplier) {
	runSpeed = _runSpeed;
	walkSpeed = _walkspeed;
	speedMultiplier = _speedMultiplier;
	swimRunSpeed = _swimRunSpeed;
	swimWalkSpeed = _swimWalkSpeed;
}

uint32_t Character::getCurrentSpeed() const {
	return (isRunning() ? runSpeed : walkSpeed) * speedMultiplier;
}

void Character::stopMove() {
	moveCount = 0;
	moving = false;
}