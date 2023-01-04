#include "ItemList.h"

#include <QListWidget>
#include <QMenu>
#include <QSortFilterProxyModel>

#include "ItemListElement.h"
#include "Bot/Bot.h"
#include "Game/Game.h"
#include "Game/Character.h"
#include "Game/MathUtils.h"

class ItemListWidgetItem : public QListWidgetItem {
	bool operator <(const QListWidgetItem& other) const
	{
		auto data = (ItemListElement*)listWidget()->itemWidget((QListWidgetItem*)this);
		auto dataOther = (ItemListElement*)listWidget()->itemWidget((QListWidgetItem*)&other);
		float ditance = data->getDistance();
		return data->getDistance() < dataOther->getDistance();
	}
};

ItemList::ItemList(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.itemList, &QListWidget::itemClicked, this, &ItemList::onItemClicked);

	ui.itemList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.itemList, &QListWidget::customContextMenuRequested, this, &ItemList::showContextMenu);
}

ItemList::~ItemList()
{

}

void ItemList::init(Game* _game, Bot* _bot) {
	game = _game;
	bot = _bot;
}

void ItemList::update(float dt) {
	auto player = game->getPlayer();
	if (player) {
		auto playerPos = player->getPosition();
		for (int i = 0; i < ui.itemList->count(); ++i)
		{
			auto* item = ui.itemList->item(i);
			auto* data = (ItemListElement*)ui.itemList->itemWidget(item);
			if (data->isValid()) {
				data->setInfo(distance(playerPos, data->getPosition()), bot->isObjectFiltered(data->getId()));
			}
			else {
				removeListItem(item);
			}
		}
	}
	ui.itemList->sortItems();
}

void ItemList::addItem(DroppedItem* item) {
	ItemListElement* itemWidget = new ItemListElement();
	itemWidget->setInfos(item, bot->isObjectFiltered(item->getId()));
	ItemListWidgetItem* listItem = new ItemListWidgetItem;

	ui.itemList->addItem(listItem);
	ui.itemList->setItemWidget(listItem, itemWidget);
}

void ItemList::removeItem(uint32_t objectId) {
	for (int i = 0; i < ui.itemList->count(); ++i)
	{
		auto* item = ui.itemList->item(i);
		auto* data = (ItemListElement*)ui.itemList->itemWidget(item);
		if (data && data->getId() == objectId) {
			removeListItem(item);
		}
	}
}

void ItemList::removeListItem(QListWidgetItem* item) {
	ui.itemList->removeItemWidget(item);
	delete item;
}

void ItemList::updateItem(DroppedItem* item) {
	auto listItem = getRowItem(item->getId());
	if (listItem) {
		auto* element = (ItemListElement*)ui.itemList->itemWidget(listItem);
		element->setInfos(item, bot->isObjectFiltered(item->getId()));
	}
}

QListWidgetItem* ItemList::getRowItem(uint32_t objectId) {
	for (int i = 0; i < ui.itemList->count(); ++i)
	{
		auto* item = ui.itemList->item(i);
		auto* data = (ItemListElement*)ui.itemList->itemWidget(item);
		if (data && data->getId() == objectId) {
			return item;
		}
	}
	return nullptr;
}

void ItemList::onItemClicked(QListWidgetItem* item) {
	auto* data = (ItemListElement*)ui.itemList->itemWidget(item);
	onItemSelected(data->getItem());
}

void ItemList::showContextMenu(const QPoint& pos)
{
	QPoint globalPos = ui.itemList->mapToGlobal(pos);
	QMenu menu;

	auto* item = ui.itemList->currentItem();
	if (item) {
		auto* data = (ItemListElement*)ui.itemList->itemWidget(item);
		rightClickedItem = data->getItem();

		auto ignoreItem = new QAction("Ignore item", this);
		connect(ignoreItem, &QAction::triggered, this, &ItemList::onIgnoreItem);
		menu.addAction(ignoreItem);

		menu.exec(globalPos);
	}
}

void ItemList::onIgnoreItem()
{
	if (rightClickedItem) {
		ignoreItem(rightClickedItem->getId());
	}
}