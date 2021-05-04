#pragma once

#include <QWidget>
#include <QStringListModel>
#include <QTimer>
#include <QElapsedTimer>
#include "ui_PlayerTab.h"

#include "Bot/Bot.h"
#include "Ipc.h"
#include "Game/Game.h"
#include "Game/IGameNotify.h"

enum class EBotState {
	Pause,
	Running,
	Detached
};

class Character;
class MainWindow;
class PlayerTab : public QWidget, IGameNotify
{
	Q_OBJECT

public:
	PlayerTab(int pid, int tabId, MainWindow* parent = Q_NULLPTR);
	int getPid();

	// IGameNotify
	void onPlayerStatusChange(Character* character) override;
	void onPlayerTargetChange(Character* target) override;
	void onPlayerSelected(LocalPlayer* player) override;
	void onCharacterSpawn(Character* character) override;
	void onCharacterUpdated(Character* character) override;
	void onCharacterRemoved(uint32_t objectId) override;
	void onDroppedItemSpawn(DroppedItem* item) override;
	void onDroppedItemRemoved(uint32_t objectId) override;
	void onSystemMessage(ESystemMessage msg) override; 
	void onPlayerTeleport() override;
	void onPartyRequest(const std::string& name) override;
	void onInventoryItemAdd(InventoryItem* item) override;
	void onInventoryItemUpdate(InventoryItem* item) override;
	void onInventoryItemRemoved(uint32_t objectId) override;

public slots:
	void logMessage(QString msg);
	void socketClosed();
	void onPacketReceived(const std::vector<char>& data, bool sent);
	void onFilterButtonClicked();
	void selectedPacketChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void charListElementSelected(Character* element);
	void itemListElementSelected(DroppedItem* item);
	void ignoreMobfamily(uint32_t npcId);
	void ignoreObject(uint32_t objectId);
	void onCloseButtonClicked();
	void updateSettings(bool value);

signals:
	void tabClose(int pid);

private:
	MainWindow* parent = nullptr;
	Ui::PlayerTab ui;
	Ipc ipc;
	Bot bot;
	Catalog catalog;
	Game game;
	PacketHandler packetHandler;
	EBotState botState = EBotState::Detached;

	QStringList logStringList;
	QStringListModel logModel;
	QTimer updateTimer;
	QElapsedTimer timeCounter;
	int pid;
	int tabId = 0;

	std::vector<char> filteredOpcode;

	void addPacketInList(const std::vector<char>& data, bool sent);
	void updateState(EBotState state);
	void toggleStart();
};
