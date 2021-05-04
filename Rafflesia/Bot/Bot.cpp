#include "Bot.h"

#include "Game/DroppedItem.h"
#include "Game/Npc.h"

#include "Qt/PlayerTab.h"

#include "TaskSerializer.h"
#include "Task/Task.h"
 
Bot::Bot(Game* _game, PacketHandler* _packetHandler, PlayerTab* _playerTab) :
	game{ _game },
	packetHandler{_packetHandler},
	playerTab {_playerTab}
{
	taskPosition = 0;

	for (int i(0); i < (int)EBotTimer::COUNT; ++i) {
		antiFloodTimers.emplace_back(0.5f);
	}
	antiFloodTimers[(int)EBotTimer::PICKUP] = 0.1f;
	antiFloodTimers[(int)EBotTimer::ACTION] = 1.0f;
	antiFloodTimers[(int)EBotTimer::ITEM] = 1.0f;

	resetPolygons();
}

bool Bot::checkFloodTimer(EBotTimer key) {
	return antiFloodTimers[(int)key].check();
}

void Bot::update(float dt) {
	if (!isRunning()) {
		return;
	}

	for (auto& timer : antiFloodTimers) {
		timer.update(dt);
	}

	
	if (player) {
		mainTask->update(dt, *game, *this);
		if (player->isMoving() && checkFloodTimer(EBotTimer::VALIDATE_POSITION)) {

		}
	}
	else {
		player = game->getPlayer();
	}
}

void Bot::updateGameInfo() {
	player = game->getPlayer();
}

bool Bot::isRunning() {
	return running;
}

void Bot::addPolygonPoint(Position point) {
	polygons[polygons.size() - 1].addPosition(point);
}

bool Bot::isInPolygons(const Position& position) {
	// Target is "inPolygon" if it is inside one "Include" polygon and outside all of the "Exclude" ones.
	bool inInclude = false;
	for (const auto& p : polygons) {
		if (p.isInside(position)) {
			if (p.getType() == EPolygon::Exclude) {
				return false;
			}
			else {
				inInclude = true;
			}
		}
	}
	return inInclude;
}

void Bot::removePolygonPosition(const PolygonPosition& id) {
	polygons[id.polygonId].removePosition(id.positionId);
}

void Bot::movePolygonPoint(const PolygonPosition& id, const Position& position) {
	polygons[id.polygonId].movePosition(id.positionId, position);
}

const std::vector<MapPolygon>& Bot::getPolygons() const {
	return polygons;
}

void Bot::toggleStart() {
	running = !running;
	if (running && player) {
		TaskSerializer s;

		auto playerName = game->getPlayer()->getName();
		if (std::strcmp(playerName.c_str(), "Hindi") == 0) {
			mainTask = s.loadTask("../Rafflesia/Data/ai/spoil.xml");
		}
		else if (std::strcmp(playerName.c_str(), "TheThunderBandit") == 0) {
			mainTask = s.loadTask("../Rafflesia/Data/ai/destro.xml");
		}
		else if (std::strcmp(playerName.c_str(), "TheVelvetThunder") == 0
			|| std::strcmp(playerName.c_str(), "Khorg") == 0) {
			mainTask = s.loadTask("../Rafflesia/Data/ai/wc.xml");
		}
		else {
			DebugBreak();
		}

		mainTask->enter(*game, *this);
	}
}

bool Bot::requestSelectTarget(Character* target, bool skipTimer) {
	if (!game->isCharacterValid(target)) {
		return false;
	}
	if (skipTimer || checkFloodTimer(EBotTimer::TARGET)) {
		packetHandler->selectTarget(target, player->getPosition());
		return true;
	}
	return false;
}

bool Bot::requestAttack(Character* target) {
	if (!game->isCharacterValid(target)) {
		return false;
	}
	if (checkFloodTimer(EBotTimer::ATTACK)) {
		packetHandler->selectTarget(target, player->getPosition());
		return true;
	}
	return false;
}

bool Bot::moveToLocation(const Position& targetPosition) {
	packetHandler->moveToLocation(player->getPosition(), targetPosition);
	if (checkFloodTimer(EBotTimer::MOVE)) {
		packetHandler->moveToLocation(player->getPosition(), targetPosition);
		return true;
	}
	return false;
}

bool Bot::requestSitStand() {
	if (checkFloodTimer(EBotTimer::ACTION)) {
		packetHandler->requestAction(EAction::SIT_STAND);
		return true;
	}
	return false;
}

bool Bot::requestPickup(const DroppedItem* item, bool skipTimer) {
	if (!game->isItemValid(item)) {
		return false;
	}

	if (skipTimer || checkFloodTimer(EBotTimer::PICKUP)) {
		packetHandler->requestPickup(item);
		return true;
	}
	return false;
}

bool Bot::requestUseSkill(uint32_t skillId) {
	if (checkFloodTimer(EBotTimer::SKILL)) {
		packetHandler->useskill(skillId);
		return true;
	}
	return false;
}

bool Bot::requestUseItem(uint32_t itemId) {
	if (checkFloodTimer(EBotTimer::ITEM)) {
		packetHandler->useItem(itemId);
		return true;
	}
	return false;
}

void Bot::answerPartyRequest(bool answer) {
	packetHandler->answerPartyRequest(answer);
}

void Bot::toggleMobFamilyFilterList(uint32_t id) {
	auto found = std::find(mobFamilyFilterList.begin(), mobFamilyFilterList.end(), id);
	if (found != std::end(mobFamilyFilterList)) {
		mobFamilyFilterList.erase(found);
	}
	else {
		mobFamilyFilterList.push_back(id);
	}
}

void Bot::toggleObjectFilterList(uint32_t id) {
	auto found = std::find(objectFilterList.begin(), objectFilterList.end(), id);
	if (found != std::end(objectFilterList)) {
		objectFilterList.erase(found);
	}
	else {
		objectFilterList.push_back(id);
	}
}

bool Bot::isFamilyFiltered(const Character* npc) const {
	return npc->isNpc() && std::find(mobFamilyFilterList.begin(), mobFamilyFilterList.end(),((Npc*)npc)->GetNpcId()) != std::end(mobFamilyFilterList);
}

bool Bot::isObjectFiltered(uint32_t id) const {
	return std::find(objectFilterList.begin(), objectFilterList.end(), id) != std::end(objectFilterList);
}

void Bot::resetPolygons() {
	polygons.clear();
	polygons.push_back(MapPolygon{});
}

void Bot::addPolygon(EPolygon polygonType) {
	auto currentPolygon = polygons[polygons.size() - 1];
	if (currentPolygon.size() > 0) {
		polygons.push_back(MapPolygon());
	}
	polygons[polygons.size() - 1].setType(polygonType);
}

void Bot::log(const std::string& msg) {
	playerTab->logMessage(msg.c_str());
}

bool Bot::findTarget() {
	if (player == nullptr) {
		return false;
	}

	Character* foundTarget = nullptr;
	float lastDistance = (std::numeric_limits<float>::max)();
	auto characters = game->getCharacters();

	if (settings.targetAggroFirst) {
		for (const auto charEntry : characters) {
			const auto character = charEntry.second;
			auto dist = distance(character->getPosition(), player->getPosition());
			const auto id = character->getId();
			if (character->getTarget() == player.get() && !character->isDead() && dist < lastDistance) {
				lastDistance = dist;
				foundTarget = character.get();
			}
		}
	}

	if (!foundTarget) {
		for (const auto charEntry : characters) {
			const auto character = charEntry.second;

			auto dist = distance(character->getPosition(), player->getPosition());
			const auto id = character->getId();
			if (id != player->getId()
				&& (!settings.useUniqueObjectFilters || !isObjectFiltered(id))
				&& (!settings.useFamilyFilters || !isFamilyFiltered(character.get()))
				&& character->isAttackable() && !character->isDead()
				&& (!settings.usePolygons || isInPolygons(character->getPosition()))) {
				if (dist < lastDistance) {
					lastDistance = dist;
					foundTarget = character.get();

					if (settings.targetAggroFirst && foundTarget->getTarget() == player.get()) {
						break;
					}
				}
			}
		}
	}

	if (foundTarget == player->getTarget()) {
		return true;
	}
	else {
		requestSelectTarget(foundTarget);
	}
	return false;
}

void Bot::attack() {
	if (auto player = game->getPlayer()) {
		if (auto target = player->getTarget()) {
			requestAttack(target);
		}
	}
}

void Bot::moveToCharacter(Character* character, int precision) {
	if (auto player = game->getPlayer() && character) {
		// TODO
	}
}

void Bot::setSettings(BotSettings _settings) {
	settings = _settings;
}

const BotSettings* Bot::getSettings() const noexcept {
	return &settings;
}