#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Game/Game.h"
#include "Game/GameData.h"
#include "Game/LocalPlayer.h"
#include "Game/MathUtils.h"
#include "Game/Network/PacketHandler.h"

#include "MapPolygon.h"

class Game;
class Npc;
class PlayerTab;
class Task;

// Used for indexing in a vector, keep it contiguous
enum class EBotTimer {
	MOVE = 0,
	SKILL,
	ITEM,
	ACTION,
	PICKUP,
	TARGET,
	ATTACK,
	VALIDATE_POSITION,

	COUNT
};

class BotTimer {
public:
	BotTimer(float limit) {
		timeCount = 0;
		timeLimit = limit;
	}
	bool check() {
		if (timeCount >= timeLimit) {
			timeCount = 0;
			return true;
		}
		return false;
	}
	void update(float dt) { timeCount += dt; }

	float timeCount;
	float timeLimit;
};

struct BotSettings {
	bool findTarget;
	bool ignoreTargetZones;
	bool useFamilyFilters;
	bool useUniqueObjectFilters;
	bool targetAggroFirst;
	bool pickupLoots;
	bool pickupBetweenFights;
	bool sweep;
	int pickupRange;
};

struct PolygonPosition {
	int polygonId = -1;
	int positionId = -1;

	bool isValid() {
		return polygonId != -1;
	}
};

class Bot
{
public:
	Bot(Game* game, PacketHandler* packetHandler, PlayerTab* _playerTab);
	~Bot();


	void update(float dt);
	void updateGameInfo();

	void resetPolygons();
	void addPolygon(EPolygon polygonType);
	void addPolygonPoint(Position point);
	void removePolygonPosition(const PolygonPosition& id);
	void movePolygonPoint(const PolygonPosition& id, const Position& position);
	bool isInPolygons(const Position& position) const;
	const std::vector<MapPolygon>& getPolygons() const;

	void toggleStart();
	bool isRunning();

	bool requestSelectTarget(Character* target, bool skipTimer = false);
	bool requestAttack(Character* target);
	bool requestSitStand();
	bool requestPickup(const DroppedItem* item, bool skipTimer = false);
	bool requestUseSkill(uint32_t skillId);
	bool requestUseItem(uint32_t itemId);
	void answerPartyRequest(bool answer);
	bool moveToLocation(const Position& targetPosition);

	bool checkFloodTimer(EBotTimer key);

	void toggleMobFamilyFilterList(uint32_t id);
	bool isFamilyFiltered(const Character* npc) const;
	void toggleObjectFilterList(uint32_t id);
	bool isObjectFiltered(uint32_t id) const;

	void setSettings(BotSettings settings);
	const BotSettings* getSettings() const noexcept;

	// lua
	void log(const std::string& msg);
	bool findTarget();
	void attack();
	void moveToCharacter(Character* character, int precision);

private:
	Game* game;
	PacketHandler* packetHandler;
	PlayerTab* playerTab;
	LocalPlayerPtr player;
	std::vector<BotTimer> antiFloodTimers;
	size_t taskPosition = 0;
	BotSettings settings;

	std::shared_ptr<Task> mainTask = nullptr;
	std::vector<std::unique_ptr<Task>> settingsTasks;

	std::vector<MapPolygon> polygons;

	std::vector<uint32_t> mobFamilyFilterList;
	std::vector<uint32_t> objectFilterList;

	bool running = false;
};

