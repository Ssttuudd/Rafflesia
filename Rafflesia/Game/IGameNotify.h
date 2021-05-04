#pragma once

class Character;
class DroppedItem;
class InventoryItem;
class LocalPlayer;
class IGameNotify {
public:
	// Player
	virtual void onPlayerSelected(LocalPlayer* player) = 0;
	virtual void onPlayerStatusChange(Character* player) = 0;
	virtual void onPlayerTargetChange(Character* target) = 0;
	virtual void onPlayerTeleport() = 0;

	// Character
	virtual void onCharacterSpawn(Character* character) = 0;
	virtual void onCharacterUpdated(Character* character) = 0;
	virtual void onCharacterRemoved(uint32_t objectId) = 0;

	// Item
	virtual void onDroppedItemSpawn(DroppedItem* item) = 0;
	virtual void onDroppedItemRemoved(uint32_t objectId) = 0;
	virtual void onInventoryItemAdd(InventoryItem* item) = 0;
	virtual void onInventoryItemUpdate(InventoryItem* item) = 0;
	virtual void onInventoryItemRemoved(uint32_t objectId) = 0;

	// System
	virtual void onSystemMessage(ESystemMessage msg) = 0;
	virtual void onPartyRequest(const std::string& name) = 0;
};