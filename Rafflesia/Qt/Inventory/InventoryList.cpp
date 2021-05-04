#include "InventoryList.h"

#include "Game/InventoryItem.h"

InventoryList::InventoryList(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

InventoryList::~InventoryList()
{
}


void InventoryList::addItem(InventoryItem* item) {
	IntentoryItemElement* itemWidget = new IntentoryItemElement();
	itemWidget->setInfos(item);

	QListWidgetItem* listItem = new QListWidgetItem;
	ui.itemList->addItem(listItem);
	ui.itemList->setItemWidget(listItem, itemWidget);
}

void InventoryList::removeItem(uint32_t objectId) {
	for (int i = 0; i < ui.itemList->count(); ++i)
	{
		auto* item = ui.itemList->item(i);
		auto* data = (IntentoryItemElement*)ui.itemList->itemWidget(item);
		if (data && data->getId() == objectId) {
			removeListItem(item);
		}
	}
}

void InventoryList::removeListItem(QListWidgetItem* item) {
	ui.itemList->removeItemWidget(item);
	delete item;
}

void InventoryList::updateItem(InventoryItem* item) {
	auto listItem = getRowItem(item->getId());
	if (listItem) {
		auto* element = (IntentoryItemElement*)ui.itemList->itemWidget(listItem);
		element->setInfos(item);
	}
}

QListWidgetItem* InventoryList::getRowItem(uint32_t objectId) {
	for (int i = 0; i < ui.itemList->count(); ++i)
	{
		auto* item = ui.itemList->item(i);
		auto* data = (InventoryItem*)ui.itemList->itemWidget(item);
		if (data && data->getId() == objectId) {
			return item;
		}
	}
	return nullptr;
}