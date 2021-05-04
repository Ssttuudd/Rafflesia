#include "Actor.h"

#include "MathUtils.h"

double Actor::getDistance(const Position& targetPos) const
{
	return distance(position, targetPos);
}

double Actor::getDistanceFromActor(const Actor* otherActor) const {
	return distance(position, otherActor->getPosition());
}

void Actor::setPosition(uint32_t posX, uint32_t posY, uint32_t posZ)
{
	position.x = posX;
	position.y = posY;
	position.z = posZ;
}

void Actor::setPosition(const Position& position)
{
	setPosition(position.x, position.y, position.z);
}

bool Actor::operator==(const Actor& rhs) {
	return getId() == rhs.getId();
}