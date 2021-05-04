#pragma once

#include <map>
#include <optional>
#include <vector>

#include "Catalog.h"
#include "Character.h"
#include "GameData.h"
#include "IGameNotify.h"
#include "DroppedItem.h"
#include "InventoryItem.h"
#include "LocalPlayer.h"
#include "Status.h"

class Game
{
public:
	Game(Catalog* catalog);

	void update(float dt);

	void addDroppedItem(DroppedItemData data);
	void addInventoryItem(InventoryItemData data);
	void removeActor(int32_t itemId);
	void addCharacter(CharacterPtr character);
	void updateStatus(uint32_t objId, const std::vector<Status>& status);
	LocalPlayerPtr getPlayer();
	void setPlayer(LocalPlayerPtr player);
	const std::map<int, CharacterPtr>& getCharacters();
	CharacterPtr getCharacter(int objectId);
	const std::map<int, DroppedItemPtr>& getItems();
	void killCharacter(uint32_t objectId, bool sweepable);
	void moveToPosition(uint32_t objectId, uint32_t destX, uint32_t destY, uint32_t destZ, uint32_t fromX, uint32_t fromY, uint32_t fromZ); 
	void setPosition(uint32_t objectId, uint32_t x, uint32_t y, uint32_t z, uint32_t heading = 0);
	void setPosition(uint32_t objectId, const Position& position);
	void teleport(uint32_t objectId, const Position& position);
	void stopMove(uint32_t objectId, uint32_t x, uint32_t y, uint32_t z, uint32_t heading);
	void setPlayerTarget(uint32_t targetId, uint32_t levelDiff);
	void setTarget(uint32_t objectId, uint32_t targetId);
	void setState(uint32_t objectId, EState state);
	void setWaitType(uint32_t objectId, EState waitType);
	bool isSweepable(uint32_t objectId);
	void systemMessage(ESystemMessage code);
	void setUserInfo(UserInfoData data);
	bool hasAggro(uint32_t characterId) const;
	bool playerHasAggro() const;
	bool isCharacterValid(const Character* character) const;
	bool isItemValid(const DroppedItem* item) const;
	Character* getFirstPlayerAggro();
	void addPartyMember(CharacterPtr character);
	void removePartyMember(CharacterPtr character);
	void requestParty(const std::string& name);

	// Observers
	void registerObserver(IGameNotify* observer);
	void removeObserver(IGameNotify* observer);

	// Skill
	void addSkill(Skill skill);
	bool hasSkill(uint32_t skillId);

	// Buff
	void addBuff(Buff buff);
	bool hasBuff(uint32_t buffId);
	void clearBuffList();

	void setInCombattatus(int32_t targetId, bool autoAttacking);
	void restart();

	// lua
	std::string getPlayerName() const;
	bool hasTarget() const;
	Character* getTarget();
	Character* getPlayerLua();

private:
	void sortCharacters();

	std::map<int, DroppedItemPtr> droppedItems;
	std::map<int, InventoryItemPtr> inventoryItems;
	std::map<int, Skill> skills;
	std::map<int, Buff> buffs;
	std::map<int, CharacterPtr> characters;

	std::vector<CharacterPtr> partyMembers;
	std::vector<IGameNotify*> observers;

	Catalog* catalog = nullptr;
	ESystemMessage lastSystemMessage;
	LocalPlayerPtr player;
};

