#pragma once

#include <memory>
#include <vector>

#include "Bot/Bot.h"
#include "Condition.h"
#include "Game/Game.h"

enum class EUpdatePolicy {
	SEQUENTIAL,
	PARALLEL
};

enum class EExitPolicy {
	ALL_TRUE,
	ANY_TRUE,
	ONE_PASS,
	NONE
};

class Task: public IGameNotify
{
public:
	using TaskPtr = std::shared_ptr<Task>;

	virtual void enter(Game& game, Bot& bot) {
		taskPosition = 0;
		player = game.getPlayer();

		if (sequentialTasks.size() > 0) {
			sequentialTasks[taskPosition]->enter(game, bot);
		}
		for (auto task : parallelTasks) {
			task->enter(game, bot);
		}
	};

	virtual void leave(Game& game) {

	}

	virtual bool update(float dt, Game& game, Bot& bot) {
		if (sequentialTasks.size() > 0) {
			if (sequentialTasks[taskPosition]->update(dt, game, bot)) {
				sequentialTasks[taskPosition]->leave(game);
				taskPosition = (taskPosition + 1) % sequentialTasks.size();
				sequentialTasks[taskPosition]->enter(game, bot);
				if (taskPosition == 0) {
					return true;
				}
			}
		}

		unsigned int trueCount = 0;
		for (auto task : parallelTasks) {
			if (task->update(dt, game, bot)) {
				trueCount++;
			}
		}

		if (exitPolicy == EExitPolicy::ONE_PASS
			|| exitPolicy == EExitPolicy::ALL_TRUE && trueCount == parallelTasks.size()
			|| exitPolicy == EExitPolicy::ANY_TRUE && trueCount > 0) {
			return true;
		}
		return false;
	}

	void addTask(TaskPtr task, EUpdatePolicy policy) {
		if (policy == EUpdatePolicy::SEQUENTIAL) {
			sequentialTasks.push_back(task);
		}
		else {
			parallelTasks.push_back(task);
		}
	}

	void addCondition(Condition::ConditionFunc condition) {
		conditions.push_back(condition);
	}

	// GAME NOTIFY
	// Only interested tasks implement these
	virtual void onPlayerSelected(LocalPlayer* player) override {}
	virtual void onPlayerStatusChange(Character* player) override {}
	virtual void onPlayerTargetChange(Character* target) override {}
	virtual void onCharacterSpawn(Character* character) override {}
	virtual void onCharacterUpdated(Character* character) override {}
	virtual void onCharacterRemoved(uint32_t objectId) override {}
	virtual void onDroppedItemSpawn(DroppedItem* item) override {}
	virtual void onDroppedItemRemoved(uint32_t objectId) override {}
	virtual void onSystemMessage(ESystemMessage msg) override {}
	virtual void onPlayerTeleport() override {}
	virtual void onPartyRequest(const std::string& name) override {}
	virtual void onInventoryItemAdd(InventoryItem* item) override {}
	virtual void onInventoryItemRemoved(uint32_t objectId) override {}
	virtual void onInventoryItemUpdate(InventoryItem* item) override {}

protected:
	std::vector<TaskPtr> sequentialTasks;
	std::vector<TaskPtr> parallelTasks;
	EUpdatePolicy policy;
	LocalPlayerPtr player;
	size_t taskPosition = 0;
	std::vector<Condition::ConditionFunc> conditions;
	EExitPolicy exitPolicy = EExitPolicy::NONE;

	bool isValid(Game& game, Bot& bot, Character* character) const {
		for (const auto c : conditions) {
			void* target = character ? character : nullptr;
			if (!c(game, bot, target)) {
				return false;
			}
		}
		return true;
	}
};

class TaskEmpty : public Task {
public:
	TaskEmpty(std::string _name, EExitPolicy _exitPolicy = EExitPolicy::NONE)  : Task()
	{
		name = _name;
		exitPolicy = _exitPolicy;
	}

private:
	std::string name;	// for debugging purpose
};