#include "InventoryItem.h"

InventoryItem::InventoryItem(const std::string& _name, InventoryItemData data) {
	name = _name;
	id = data.id;
	update(data);
}

void InventoryItem::update(InventoryItemData data) {
	itemId = data.itemId;
	count = data.count;
	type = data.type;
	equipped = data.equipped;
}