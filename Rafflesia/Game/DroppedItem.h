#pragma once

#include <memory>
#include <string>

#include "Actor.h"

struct DroppedItemData : public ActorData {
	uint32_t dropper = 0;
	uint32_t itemId = 0;
	uint32_t amount = 0;
	uint8_t stackable = 0;
	std::string name;
};

class DroppedItem;
using DroppedItemPtr = std::shared_ptr<DroppedItem>;

class DroppedItem : public Actor
{
public:
	DroppedItem(DroppedItemData data);
	uint32_t getDropper() const { return dropper; }
	const std::string& getName() const { return name; }

protected:
	uint32_t itemId = 0;
	uint32_t amount = 0;
	uint32_t dropper = 0;
	const std::string name;
};