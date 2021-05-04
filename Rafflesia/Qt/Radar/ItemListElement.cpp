#include "ItemListElement.h"

#include "Game/DroppedItem.h"

ItemListElement::ItemListElement(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.filteredLabel->setVisible(false);
}

ItemListElement::~ItemListElement()
{
}

void ItemListElement::setInfos(DroppedItem* _item, bool filtered) {
	item = _item;
	objectId = item->getId();
	ui.itemName->setText(_item->getName().c_str());
	ui.filteredLabel->setVisible(filtered);
}

uint32_t ItemListElement::getId() {
	return objectId;
}

bool ItemListElement::isValid() const {
	return item != nullptr;
}

const float ItemListElement::getDistance() const {
	return distance;
}

void ItemListElement::setInfo(float value, bool filtered) {
	distance = value;
	ui.filteredLabel->setVisible(filtered);
}

Position ItemListElement::getPosition() const {
	if (item)
		return item->getPosition();

	return { 0, 0, 0 };
}

DroppedItem* ItemListElement::getItem() const {
	return item;
}