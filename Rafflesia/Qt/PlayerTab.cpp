#include "PlayerTab.h"

#include "QHexView.h"
#include "utils.h"

#include "Game/Character.h"
#include "Game/Npc.h"

#include <QSettings.h>
#include "MainWindow.h"
#include "QtUtils.h"

PlayerTab::PlayerTab(int _pid, int _tabId, MainWindow* parent)
	: QWidget(parent),
	parent(parent),
	packetHandler(game, &ipc),
	bot(&game, &packetHandler, this),
	game(&catalog)
{
	qRegisterMetaType< std::vector<char> >("std::vector<char>");

	game.registerObserver(this);
	ui.setupUi(this);
	ui.radarWidget->init(&game, &bot);
	ui.charListWidget->init(&game, &bot);
	ui.itemListWidget->init(&game, &bot);

	pid = _pid;
	tabId = _tabId;

	updateState( EBotState::Detached );
	
	if (!ipc.connect(pid)) {
		logMessage("Could not connect to the game... Stopping there.");
		return;
	}

	// Ipc signals
	//connect(&ipc, &Ipc::sendMessage, this, &PlayerTab::logMessage);
	connect(&ipc, &QThread::finished, this, &PlayerTab::socketClosed);
	connect(&ipc, &Ipc::onPacketData, this, &PlayerTab::onPacketReceived);
	connect(&ipc, &Ipc::onGameInfos, this, &PlayerTab::onGameInfos );
	connect(&ipc, &Ipc::onRootConnected, this, &PlayerTab::onRootConnected );
	connect(ui.filterButton, &QPushButton::clicked, this, &PlayerTab::onFilterButtonClicked);
	connect(ui.closeButton, &QPushButton::clicked, this, &PlayerTab::onCloseButtonClicked);
	connect(ui.playButton, &QPushButton::clicked, this, &PlayerTab::toggleStart);

	connect( ui.findTarget, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.ignoreTargetZones, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.useFamilyFilters, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.useOBjectFilters, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.targetAggroFirst, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.pickupLoots, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.sweep, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.pickupBetweenFights, &QCheckBox::toggled, this, &PlayerTab::onSettingsChangedBool );
	connect( ui.pickupRange, &QSpinBox::valueChanged, this, &PlayerTab::onSettingsChangedInt );

	connect(ui.testButton, &QPushButton::clicked, [&]() {
		bot.moveToLocation(game.getPlayer()->getTarget()->getPosition());
	});

	ui.testButton->setVisible( false );

	// UI signals
	connect(ui.packetList, &QListWidget::currentItemChanged, this, &PlayerTab::selectedPacketChanged);
	connect(ui.charListWidget, &CharacterList::onCharacterSelected, this, &PlayerTab::charListElementSelected);
	connect(ui.charListWidget, &CharacterList::ignoreMobFamily, this, &PlayerTab::ignoreMobfamily);
	connect(ui.charListWidget, &CharacterList::ignoreMob, this, &PlayerTab::ignoreObject);

	connect(ui.itemListWidget, &ItemList::onItemSelected, this, &PlayerTab::itemListElementSelected);
	connect(ui.itemListWidget, &ItemList::ignoreItem, this, &PlayerTab::ignoreObject);

	ui.targetWidget->setVisible( false );
	ui.playerWidget->setVisible( false );

	//ui.logTextList->setModel(&logModel);

	ipc.start();

	connect(&updateTimer, &QTimer::timeout, [this]() {

		const float dt = (float)(timeCounter.elapsed()) / 1000;
		bot.update(dt);
		game.update(dt);
		ui.charListWidget->update(dt);
		ui.itemListWidget->update(dt);
		timeCounter.restart();

		if (bot.isRunning() && botState != EBotState::Running) {
			updateState(EBotState::Running);
		}
		else if (!bot.isRunning() && botState == EBotState::Running){
			updateState(EBotState::Pause);
		}

		const float timeBetweenGameInfoRequests = 0.1f;
		static float timeSinceLastRequestGameInfo = timeBetweenGameInfoRequests;
		if( timeSinceLastRequestGameInfo <= 0.0f )
		{
			//packetHandler.requestGameInfos();
			timeSinceLastRequestGameInfo = timeBetweenGameInfoRequests;;
		}
		timeSinceLastRequestGameInfo -= dt;
	});
	updateTimer.start();

	updateBotSettings();
}

void PlayerTab::stop()
{
	ipc.stop();
	ipc.exit();
	ipc.wait();
	ipc.deleteLater();
}

void PlayerTab::logMessage(QString msg)
{
	ui.logWindow->append( msg );
}

void PlayerTab::toggleStart() {
	bot.toggleStart();
	if (bot.isRunning()) {
		updateState(EBotState::Running);
		ui.playButton->setText("Pause");
	}
	else {
		updateState(EBotState::Pause);
		ui.playButton->setText("Start");
	}
}

void PlayerTab::socketClosed()
{
	logMessage(QString("Root disconnected."));
	updateState(EBotState::Detached);
	qDebug("socket closed\n");
}

int PlayerTab::getPid() {
	return pid;
}

void PlayerTab::onFilterButtonClicked() {
	const auto* item = ui.packetList->currentItem();
	if (item) {
		filteredOpcode.push_back(item->data(Qt::UserRole).toByteArray()[2]);
	}
}

void PlayerTab::onPacketReceived(const std::vector<char>& data, bool sent) 
{
	std::string packetName;
	if( APacket * packet{ packetHandler.getPacket( data, sent ) } )
	{
		PacketIStream stream( data );
		uint16_t length;
		uint8_t opcode;
		stream >> length >> opcode;

		packet->parse( stream, game );
		packetName = packet->getName( data );
	}
	else
	{
		packetName = APacket::getDefaultName( data );
	}

	if( (sent && ui.skipSnt->checkState() == Qt::CheckState::Checked)
		|| (!sent && ui.skipRcv->checkState() == Qt::CheckState::Checked) ) {
		return;
	}

	if( data[2] == 0x8f ) {
		qDebug( hexStr( (unsigned char*)data.data(), data.size() ).c_str() );
	}

	for( const auto code : filteredOpcode ) {
		if( data[2] == code ) {
			return;
		}
	}

	QListWidgetItem* newItem = new QListWidgetItem;
	newItem->setData( Qt::UserRole, QByteArray( (char*)data.data(), data.size() ) );
	newItem->setText( packetName.c_str() );
	newItem->setBackground( sent ? Qt::white : Qt::lightGray );
	ui.packetList->addItem( newItem );
}

void PlayerTab::onGameInfos( const Position& playerPosition )
{
	game.setPlayerPosition( playerPosition );
}

void PlayerTab::onRootConnected()
{
	updateState( EBotState::Pause );
	logMessage( "Root is connected." );
}

void PlayerTab::selectedPacketChanged(QListWidgetItem* current, QListWidgetItem* previous) {
	if (current) {
		QVariant data = current->data(Qt::UserRole);
		QHexView::DataStorageArray* storage = new QHexView::DataStorageArray(data.toByteArray());
		ui.packetViewer->setData(storage);
		ui.packetViewer->repaint();
	}
	else {
		ui.packetViewer->clear();
		ui.packetViewer->repaint();
	}
}

void PlayerTab::charListElementSelected(Character* character) {
	bot.requestSelectTarget(character, true);
}

void PlayerTab::itemListElementSelected(DroppedItem* item) {
	bot.requestPickup(item, true);
}

void PlayerTab::ignoreMobfamily(uint32_t npcId) {
	bot.toggleMobFamilyFilterList(npcId);
}

void PlayerTab::ignoreObject(uint32_t objectId) {
	bot.toggleObjectFilterList(objectId);
}

// Game notifications
void PlayerTab::onPlayerStatusChange(Character* character) {
	updateBar(ui.healthBar, character->getHp(), character->getMaxHp());
	updateBar(ui.manaBar, character->getMp(), character->getMaxMp());
}

void PlayerTab::onPlayerTargetChange(Character* target) {
	if (target) {
		updateBar(ui.targetHealthBar, target->getHp(), target->getMaxHp());
		updateBar(ui.targetManaBar, target->getMp(), target->getMaxMp());
		ui.targetName->setText(target->getName().c_str());
		if (target->isNpc()) {
			ui.npcId->setText(QString::number(((Npc*)target)->GetNpcId()));
		}
		ui.charListWidget->targetCharacter(target);
		ui.targetWidget->setVisible( true );
	}
	else {
		ui.charListWidget->removeCurrentTarget();
		ui.targetWidget->setVisible( false );
	}
}

void PlayerTab::onPlayerSelected(LocalPlayer* player) {

	QSettings settings( getCharacterSettingsPath( player->getName() ), getAppName() );
	ui.findTarget->setChecked( settings.value( "findTarget" ).toBool() );
	ui.ignoreTargetZones->setChecked( settings.value( "ignoreTargetZones" ).toBool() );
	ui.useFamilyFilters->setChecked( settings.value( "useFamilyFilters" ).toBool() );
	ui.useOBjectFilters->setChecked( settings.value( "useOBjectFilters" ).toBool() );
	ui.targetAggroFirst->setChecked( settings.value( "targetAggroFirst" ).toBool() );
	ui.pickupLoots->setChecked( settings.value( "pickupLoots" ).toBool() );
	ui.pickupBetweenFights->setChecked( settings.value( "pickupBetweenFights" ).toBool() );
	ui.pickupRange->setValue( settings.value( "pickupRange" ).toInt() );

	ui.playerWidget->setVisible( true );

	parent->setTabName(tabId, player->getName() + " " + std::to_string(pid));
	bot.updateGameInfo();
	updateBotSettings();
}

void PlayerTab::onCharacterSpawn(Character* character) {
	ui.charListWidget->addCharacter(character);
}

void PlayerTab::onCharacterUpdated(Character* character) {
	ui.charListWidget->updateCharacter(character);
}

void PlayerTab::onCharacterRemoved(uint32_t objectId) {
	ui.charListWidget->removeCharacter(objectId);
}

void PlayerTab::onDroppedItemSpawn(DroppedItem* item) {
	ui.itemListWidget->addItem(item);
}

void PlayerTab::onDroppedItemRemoved(uint32_t objectId) {
	ui.itemListWidget->removeItem(objectId);
}

void PlayerTab::onSystemMessage(ESystemMessage code) {
	std::string message{ "" };
	switch (code) {
	case ESystemMessage::ALREADY_SPOILED:
		message = "System: Target is already spoiled.";
		break;
	case ESystemMessage::SPOIL_ACTIVATED:
		message = "System: Spoil condition activated.";
		break;
	case ESystemMessage::CANNOT_SEE_TARGET:
		message = "System: Cannot see target.";
		break;
	case ESystemMessage::DISTANCE_TOO_FAR:
		message = "System: Distance is too far to cast.";
		break;
	case ESystemMessage::INVALID_TARGET:
		message = "System: Invalid target.";
		break;
	}
	logMessage(message.c_str());
}

void PlayerTab::updateState(EBotState state) {
	switch (state) {
	case EBotState::Detached:
	{
		ui.stateText->setText("Detached");
		ui.stateIcon->setPixmap(QPixmap(":/icons/detachedIcon.png"));
	}
	break;
	case EBotState::Pause:
	{
		ui.playButton->setText("Start");
		ui.stateText->setText("Paused");
		ui.stateIcon->setPixmap(QPixmap(":/icons/pauseIcon.png"));
	}
	break;
	case EBotState::Running:
	{
		ui.playButton->setText("Pause");
		ui.stateText->setText("Running");
		ui.stateIcon->setPixmap(QPixmap(":/icons/runningIcon.png"));
	}
	break;
	}
}

void PlayerTab::onCloseButtonClicked() {
	tabClose(pid);
}

void PlayerTab::onPlayerTeleport() {
	ui.radarWidget->resetMap();
}

void PlayerTab::onPartyRequest(const std::string& name) {

}

void PlayerTab::onInventoryItemAdd(InventoryItem* item) {
	ui.inventoryList->addItem(item);
}

void PlayerTab::onInventoryItemUpdate(InventoryItem* item) {
	ui.inventoryList->updateItem(item);
}

void PlayerTab::onInventoryItemRemoved(uint32_t objectId) {
	ui.inventoryList->removeItem(objectId);
}

void PlayerTab::onSettingsChangedBool(bool value) {
	updateBotSettings();
	saveSettings();
}

void PlayerTab::onSettingsChangedInt( int value ) {
	updateBotSettings();
	saveSettings();
}

void PlayerTab::updateBotSettings()
{
	bot.setSettings( {
		ui.findTarget->isChecked(),
		ui.ignoreTargetZones->isChecked(),
		ui.useFamilyFilters->isChecked(),
		ui.useOBjectFilters->isChecked(),
		ui.targetAggroFirst->isChecked(),
		ui.pickupLoots->isChecked(),
		ui.pickupBetweenFights->isChecked(),
		ui.sweep->isChecked(),
		ui.pickupRange->value()
	} );
}

void PlayerTab::saveSettings()
{

	const auto player = game.getPlayer();
	if( player && !player->getName().empty() )
	{
		QSettings settings( getCharacterSettingsPath( player->getName() ), getAppName() );
		settings.setValue( "findTarget", ui.findTarget->isChecked() );
		settings.setValue( "ignoreTargetZones", ui.ignoreTargetZones->isChecked() );
		settings.setValue( "useFamilyFilters", ui.useFamilyFilters->isChecked() );
		settings.setValue( "useOBjectFilters", ui.useOBjectFilters->isChecked() );
		settings.setValue( "targetAggroFirst", ui.targetAggroFirst->isChecked() );
		settings.setValue( "pickupLoots", ui.pickupLoots->isChecked() );
		settings.setValue( "pickupBetweenFights", ui.pickupBetweenFights->isChecked() );
		settings.setValue( "pickupRange", ui.pickupRange->value() );
	}
}