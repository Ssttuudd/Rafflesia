#include "CharacterList.h"

#include <QListWidget>
#include <QMenu>
#include <QSortFilterProxyModel>

#include "Bot/Bot.h"
#include "CharListElement.h"
#include "Game/Game.h"
#include "Game/Character.h"
#include "Game/MathUtils.h"
#include "Game/Npc.h"

class CharListWidgetItem : public QListWidgetItem {
	bool CharListWidgetItem::operator <(const QListWidgetItem& other) const
	{
		auto data = (CharListElement*)listWidget()->itemWidget((QListWidgetItem*)this);
		auto dataOther = (CharListElement*)listWidget()->itemWidget((QListWidgetItem*)&other);
		float ditance = data->getDistance();
		return data->getDistance() < dataOther->getDistance();
	}
};

CharacterList::CharacterList(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.charList, &QListWidget::itemClicked, this, &CharacterList::onItemClicked);
	ui.charList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.charList, &QListWidget::customContextMenuRequested, this, &CharacterList::showContextMenu);
}

CharacterList::~CharacterList()
{

}

void CharacterList::init(Game* _game, Bot* _bot) {
	game = _game;
	bot = _bot;
}

void CharacterList::update(float dt) {
	auto player = game->getPlayer();
	if (player) {
		auto playerPos = player->getPosition();
		for (int i = 0; i < ui.charList->count(); ++i)
		{
			auto* item = ui.charList->item(i);
			auto* data = (CharListElement*)ui.charList->itemWidget(item);
			if (data->isValid() && data->getCharacter()) {
				const auto* character = data->getCharacter();
				bool familyFiltered = character->isNpc() && bot->isFamilyFiltered((Npc*)character);
				data->setInfo(distance(playerPos, data->getPosition()), familyFiltered, bot->isObjectFiltered(character->getId()));
			}
			else {
				removeListItem(item);
			}
		}
	}
	ui.charList->sortItems();
}

void CharacterList::addCharacter(Character* character) {
	CharListElement* charWidget = new CharListElement();
	charWidget->setInfos(character);
	CharListWidgetItem* item = new CharListWidgetItem;

	ui.charList->addItem(item);
	ui.charList->setItemWidget(item, charWidget);
}

void CharacterList::removeCurrentTarget() {
	auto item = getRowItem(currentTargetId);
	if (item) {
		auto targetChar = (CharListElement*)ui.charList->itemWidget(item);
		targetChar->setTargeted(false);
	}
}

void CharacterList::targetCharacter(Character* target) {
	removeCurrentTarget();
	// item can be null if we target something that's not in the list (like the local player or something that died)
	currentTargetId = target->getId();
	auto item = getRowItem(currentTargetId);
	if (item) {
		auto targetChar = (CharListElement*)ui.charList->itemWidget(item);
		targetChar->setTargeted(true);
	}
}

void CharacterList::removeCharacter(uint32_t objectId) {
	for (int i = 0; i < ui.charList->count(); ++i)
	{
		auto* item = ui.charList->item(i);
		auto* data = (CharListElement*)ui.charList->itemWidget(item);
		if (data && data->getId() == objectId) {
			removeListItem(item);
		}
	}
}

void CharacterList::removeListItem(QListWidgetItem* item) {
	ui.charList->takeItem(ui.charList->row(item));
	delete item;
}

void CharacterList::updateCharacter(Character* character) {
	auto item = getRowItem(character->getId());
	if (item) {
		auto* element = (CharListElement*)ui.charList->itemWidget(item);
		element->setInfos(character);
	}
}

QListWidgetItem* CharacterList::getRowItem(uint32_t objectId) {
	for (int i = 0; i < ui.charList->count(); ++i)
	{
		auto* item = ui.charList->item(i);
		auto* data = (CharListElement*)ui.charList->itemWidget(item);
		if (data && data->getId() == objectId) {
			return item;
		}
	}
	return nullptr;
}

void CharacterList::onItemClicked(QListWidgetItem* item) {
	auto* data = (CharListElement*)ui.charList->itemWidget(item);
	onCharacterSelected(data->getCharacter());
}

void CharacterList::showContextMenu(const QPoint& pos)
{
	QPoint globalPos = ui.charList->mapToGlobal(pos);
	QMenu menu;

	auto* item = ui.charList->currentItem();
	if (item) {
		auto* data = (CharListElement*)ui.charList->itemWidget(item);
		rightClickedCharacter = data->getCharacter();
		bool isNpc = rightClickedCharacter->isNpc();

		if (rightClickedCharacter->isNpc() && rightClickedCharacter->isAttackable()) {
			auto ignoreFamilyAction = new QAction("Ignore mob family", this);
			connect(ignoreFamilyAction, &QAction::triggered, this, &CharacterList::onIgnoreMobFamilyAction);
			menu.addAction(ignoreFamilyAction);
		}

		auto ignoreMob = new QAction("Ignore character", this);
		connect(ignoreMob, &QAction::triggered, this, &CharacterList::onIgnoreMobAction);
		menu.addAction(ignoreMob);

		menu.exec(globalPos);
	}
}

void CharacterList::onIgnoreMobFamilyAction() 
{
	if (rightClickedCharacter) {
		auto npc = (Npc*)rightClickedCharacter;
		ignoreMobFamily(npc->GetNpcId());
	}
}

void CharacterList::onIgnoreMobAction()
{
	if (rightClickedCharacter) {
		ignoreMob(rightClickedCharacter->getId());
	}
}
