#include "DroppedItem.h"

#include "Shared/SimpleBinStream.hpp"

DroppedItem::DroppedItem(DroppedItemData data) :
	name{ data.name },
	dropper{ data.dropper },
	itemId{ data.itemId },
	amount{ data.amount }
{
	id = data.id;
	position = data.position;
}