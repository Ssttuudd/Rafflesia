#pragma once

#include <memory>
#include <string>

#include "Actor.h"

enum class ItemType {
	Weapon,
	Armor,
	Jewel,
	Quest,
	Adena,
	Item
};

enum class ItemGrade {
	None,
	D,
	C,
	B,
	A,
	S,
	S80,
	S84,
};

struct InventoryItemData : public ActorData {
	uint32_t itemId = 0;
	uint32_t count = 0;
	uint8_t inventoryPosition;
	ItemType type;
	uint32_t boddyPart;
	bool equipped;
};

class InventoryItem;
using InventoryItemPtr = std::shared_ptr<InventoryItem>;

class InventoryItem: public Actor
{
public:
	InventoryItem(const std::string& name, InventoryItemData data);
	std::string getName() const { return name; }
	bool isEquipped() const { return equipped; }
	uint32_t getCount() const { return count; }
	void update(InventoryItemData data);

protected:
	std::string name;
	uint32_t itemId;
	uint32_t count;
	uint32_t price;
	ItemType type;
	bool equipped;
	bool isQuest;
};

