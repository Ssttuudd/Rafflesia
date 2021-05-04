#include "Game.h"

#include <algorithm>
#include <functional>

#include "MathUtils.h"
#include "Npc.h"
#include "Status.h"

#define NOTIFY(function, args)		std::for_each(observers.begin(), observers.end(), [&](auto* elem) { return elem->function(args); });
#define NOTIFY_NO_ARG(function)		std::for_each(observers.begin(), observers.end(), [&](auto* elem) { return elem->function(); });

Game::Game(Catalog* _catalog):
	lastSystemMessage(ESystemMessage::NONE),
	catalog{ _catalog }
{

}

void Game::update(float dt) {
	for (auto pair : characters) {
		pair.second->update(dt);
	}
}

void Game::addDroppedItem(DroppedItemData data) {
	auto item = std::make_shared<DroppedItem>(data);
	data.name = catalog->getItemInfo(data.itemId)->name;
	droppedItems.emplace(data.id, item);
	NOTIFY(onDroppedItemSpawn, item.get());
}

void Game::addInventoryItem(InventoryItemData data) {
	const auto* itemInfo = catalog->getItemInfo(data.itemId);
	auto item = std::make_shared<InventoryItem>(itemInfo->name, data);
	if (inventoryItems.find(data.id) != inventoryItems.end()) {
		inventoryItems[data.id]->update(data);
		NOTIFY(onInventoryItemUpdate, item.get());
	}
	else {
		inventoryItems.emplace(data.id, item);
		NOTIFY(onInventoryItemAdd, item.get());
	}
}

void Game::removeActor(int32_t objectId) {
	if (droppedItems.find(objectId) != std::end(droppedItems)) {
		droppedItems.erase(objectId);
		NOTIFY(onDroppedItemRemoved, objectId);
	}
	else if (characters.find(objectId) != std::end(characters)) {
		if (player) {
			auto target = player->getTarget();
			if (target && target->getId() == objectId) {
				player->resetTarget();
			}
		}
		characters.erase(objectId);
		NOTIFY(onCharacterRemoved, objectId);
	}
}

void Game::addCharacter(CharacterPtr character) {
	characters.emplace(character->getId(), character);
	if (player && character->getId() != player->getId()) {
		if (character->isNpc()) {
			const auto* npcInfo = catalog->getNpcInfo(((Npc*)character.get())->GetNpcId());
			if (npcInfo) {
				character->setName(npcInfo->name);
				character->setSpeed(npcInfo->runSpeed, npcInfo->walkSpeed, 0, 0, 1);
			}
		}
		NOTIFY(onCharacterSpawn, character.get());
	}
}

void Game::setPlayer(LocalPlayerPtr _player) {
	player = _player;
	if (characters.find(_player->getId()) != characters.end()) {
		auto character = characters[_player->getId()];
		player->init(character);
	}
	characters[player->getId()] = player;
	NOTIFY(onPlayerSelected, player.get());
}

void Game::updateStatus(uint32_t objId, const std::vector<Status>& status)
{
	if(characters.find(objId) == characters.end()) {
		return;
	}

	auto character = characters[objId];

	for (auto& s : status)
	{
		switch ((EStatus)s.type) {
		case EStatus::CUR_HP:
			character->setHp(s.value);
			break;
		case EStatus::MAX_HP:
			character->setMaxHp(s.value);
			break;
		case EStatus::CUR_MP:
			character->setMp(s.value);
			break;
		case EStatus::MAX_MP:
			character->setMaxMp(s.value);
			break;
		case EStatus::CUR_CP:
			character->setCp(s.value);
			break;
		case EStatus::MAX_CP:
			character->setMaxCp(s.value);
			break;
		case EStatus::LEVEL:
			character->setLevel(s.value);
			break;
		}
	}

	if (player) {
		if (objId == player->getId()) {
			NOTIFY(onPlayerStatusChange, player.get());
		}
		else {
			auto target = player->getTarget();
			if (target && objId == target->getId()) {
				NOTIFY(onPlayerTargetChange, target);
			}
			NOTIFY(onCharacterUpdated, character.get());
		}
	}
}

LocalPlayerPtr Game::getPlayer() {
	if (!player || player->getId() == 0) {
		return nullptr;
	}
	return player;
}

const std::map<int, CharacterPtr>& Game::getCharacters() {
	return characters;
}

CharacterPtr Game::getCharacter(int objectId) {
	if (characters.find(objectId) != characters.end()) {
		return characters[objectId];
	}
	return nullptr;
}

const std::map<int, DroppedItemPtr>& Game::getItems() {
	return droppedItems;
}

void Game::killCharacter(uint32_t objectId, bool sweepable) {
	if (characters.find(objectId) != characters.end()) {
		auto character = characters[objectId];
		character->setSweepable(sweepable);
		character->setDead();
		if (player && player->getTarget() == character.get()) {
			NOTIFY(onPlayerTargetChange, character.get());
		}
	}
}

void Game::moveToPosition(uint32_t objectId, uint32_t destX, uint32_t destY, uint32_t destZ, uint32_t fromX, uint32_t fromY, uint32_t fromZ)
{
	if (characters.find(objectId) != characters.end()) {
		auto character = characters[objectId];
		character->setPosition(fromX, fromY, fromZ);
		if (destX != destY != destZ != 0) {
			character->setDestination(destX, destY, destZ);
		}
	}
}

void Game::setPosition(uint32_t objectId, uint32_t x, uint32_t y, uint32_t z, uint32_t heading) {
	if (characters.find(objectId) != characters.end()) {
		auto character = characters[objectId];
		character->setPosition(x, y, z);
		character->setHeading(heading);
	}
}

void Game::setPosition(uint32_t objectId, const Position& position) {
	setPosition(objectId, position.x, position.y, position.z, position.heading);
}

void Game::teleport(uint32_t objectId, const Position& position) {
	setPosition(objectId, position);
	if (player && objectId == player->getId()) {
		NOTIFY_NO_ARG(onPlayerTeleport);
	}
}

void Game::stopMove(uint32_t objectId, uint32_t x, uint32_t y, uint32_t z, uint32_t heading) {
	if (characters.find(objectId) != characters.end()) {
		auto character = characters[objectId];
		character->setPosition(x, y, z);
		character->setHeading(heading);
		character->stopMove();
	}
}

void Game::setWaitType(uint32_t objectId, EState waitType) {
	if (characters.find(objectId) != characters.end()) {
		auto character = characters[objectId];
		character->setState(waitType);
	}
}

void Game::setPlayerTarget(uint32_t targetId, uint32_t levelDiff) {
	if (player && characters.find(targetId) != characters.end()) {
		auto target = characters[targetId];
		player->setTarget(target.get());
		target->setLevel(player->getLevel() - levelDiff);
		NOTIFY(onPlayerTargetChange, target.get());
	}
}

void Game::setTarget(uint32_t objectId, uint32_t targetId) {
	if (characters.find(objectId) != characters.end()) {
		if (characters.find(targetId) != characters.end()) {
			auto character = characters[objectId];
			auto target = characters[targetId];
			character->setTarget(target.get());
		}
		else if (targetId == 0) {
			player->setTarget(nullptr);
		}
	}
}

bool Game::isSweepable(uint32_t objectId) {
	if (characters.find(objectId) != characters.end()) {
		return characters[objectId]->isSweepable();
	}
	return false;
}

void Game::addSkill(Skill skill) {
	skills[skill.id] = skill;
}

bool Game::hasSkill(uint32_t skillId) {
	return skills.find(skillId) != std::end(skills);
}

void Game::addBuff(Buff buff) {
	buffs[buff.id] = buff;
}

bool Game::hasBuff(uint32_t buffId) {
	return buffs.find(buffId) != std::end(buffs);
}

void Game::clearBuffList() {
	buffs.clear();
}

void Game::systemMessage(ESystemMessage code) {
	switch (code) {
	case ESystemMessage::ALREADY_SPOILED:
	case ESystemMessage::SPOIL_ACTIVATED:
		if (player && player->getTarget()) {
			player->getTarget()->setSpoiled(true);
		}
		break;
	}
	NOTIFY(onSystemMessage, code);
}

void Game::setUserInfo(UserInfoData data) {
	CharacterPtr character;
	if (characters.find(data.id) != characters.end()) {
		character = characters[data.id];
	}
	else {
		auto nameResult = std::find_if(characters.begin(), characters.end(), [&data](const auto& charPair) {
			return strcmp(charPair.second->getName().c_str(), data.name.c_str()) == 0;
		});

		if (nameResult != std::end(characters)) {
			character = nameResult->second;

			// On restart the CHAR_SELECTED packet doesn't contain the correct id. UserInfo provides it.
			// If we find a character that has the exact same name but not the same ID, we update it.
			if (data.id != character->getId()) {
				characters.erase(nameResult);
				characters[data.id] = character;
				character->setId(data.id);
			}
		}
		else{
			character = std::make_shared<Player>(data);
			addCharacter(character);
		}
	}
	
	character->setPosition({ (int32_t)data.posX, (int32_t)data.posY, (int32_t)data.posZ });
	character->setHp(data.curHp);
	character->setMaxHp(data.maxHp);
	character->setMp(data.curMp);
	character->setMaxMp(data.maxMp);
	character->setCp(data.curCp);
	character->setMaxCp(data.maxCp);
	character->setLevel(data.level);
	character->setSpeed(data.runSpeed, data.walkSpeed, 
		data.swimRunSpeed, data.swimWalkSpeed, data.speedMultiplier);

	if (data.name.size() > 1) {
		character->setName(data.name);
	}

	if (player && data.id == player->getId()) {
		NOTIFY(onPlayerStatusChange, player.get());
	}
}

void Game::setState(uint32_t objectId, EState state) {
	if (characters.find(objectId) != characters.end()) {
		characters[objectId]->setState(state);
	}
}

void Game::setInCombattatus(int32_t targetId, bool autoAttacking) {
	if (player && characters.find(targetId) != characters.end()) {
		auto target = characters[targetId];
		player->setInCombat(autoAttacking);
	}
}

bool Game::hasAggro(uint32_t characterId) const {
	for (auto pair : characters) {
		auto target = pair.second->getTarget();
		if (target && target->getId() == characterId) {
			return true;
		}
	}
	return false;
}

bool Game::playerHasAggro() const {
	if (player) {
		auto playerId = player->getId();
		for (auto pair : characters) {
			if (pair.second->isAlive()) {
				auto target = pair.second->getTarget();
				if (target && target->getId() == playerId) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Game::isCharacterValid(const Character* character) const {
	return character && characters.find(character->getId()) != characters.end();
}

bool Game::isItemValid(const DroppedItem* item) const {
	return item && droppedItems.find(item->getId()) != droppedItems.end();
}

Character* Game::getFirstPlayerAggro() {
	if (player) {
		auto playerId = player->getId();
		for (auto pair : characters) {
			auto& character = pair.second;
			if (character->isAlive() && character->isNpc()) {
				auto target = character->getTarget();
				if (target && target->getId() == playerId) {
					return character.get();
				}
			}
		}
	}
	return nullptr;
}

void Game::restart() {
	player = nullptr;
	droppedItems.clear();
	characters.clear();
}

void Game::registerObserver(IGameNotify* observer) {
	observers.push_back(observer);
}

void Game::removeObserver(IGameNotify* observer) {
	observers.erase(std::remove(observers.begin(), observers.end(), observer));
}

void Game::addPartyMember(CharacterPtr character) {
	partyMembers.push_back(character);
}

void Game::removePartyMember(CharacterPtr character) {
	partyMembers.erase(std::remove(partyMembers.begin(), partyMembers.end(), character));
}

void Game::requestParty(const std::string& name) {
	NOTIFY(onPartyRequest, name);
}

std::string Game::getPlayerName() const {
	if (player) {
		return player->getName();
	}
	return "InvalidPlayer";
}

bool Game::hasTarget() const {
	return player && player->getTarget();
}

Character* Game::getTarget() {
	return player ? player->getTarget() : nullptr;
}

Character* Game::getPlayerLua() {
	if (!player || player->getId() == 0) {
		return nullptr;
	}
	return player.get();
}