#pragma once

#include <string>

#include "Task.h"

class TaskAcceptParty : public Task
{
public:
	TaskAcceptParty(const std::string& _name) :
		name{ _name },
		game{ nullptr } {
	}

	~TaskAcceptParty() {
		game->removeObserver(this);
	}

	void enter(Game& _game, Bot& bot) {
		Task::enter(_game, bot);
		game = &_game;
		game->registerObserver(this);
	}

	bool update(float dt, Game& _game, Bot& bot) {
		if (acceptRequest) {
			// If we send an accept party packet, the client will spamm packets to the server
			//bot.answerPartyRequest(true);
		}
		return true;
	}

	void onPartyRequest(const std::string& _name) override {
		if (name == _name || name == "*") {
			acceptRequest = true;
		}
	}

private:
	bool acceptRequest = false;
	const std::string name;
	Game* game;
};