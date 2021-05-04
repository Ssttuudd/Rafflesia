#include "IntentoryItemElement.h"

#include <QDir>

#include "Game/Actor.h"

IntentoryItemElement::IntentoryItemElement(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

IntentoryItemElement::~IntentoryItemElement()
{
}
/*
std::string getIconPath(InventoryItem* _item) {
	return QDir::currentPath() + "Data/Icons/L2/";
}*/

void IntentoryItemElement::setInfos(InventoryItem* _item) {
	item = _item;
	objectId = item->getId();

	auto count = _item->getCount();
	QString kilos = "";
	while (count > 1000) { count /= 1000; kilos += "k"; }
	ui.countLabel->setText(QString::number(count) + kilos);


	ui.nameLabel->setText(_item->getName().c_str());
	if (item->isEquipped()) {
		QPalette pal = palette();
		pal.setColor(QPalette::Base, Qt::lightGray);
		setAutoFillBackground(true);
		setPalette(pal);
	}
}

uint32_t IntentoryItemElement::getId() {
	return objectId;
}

bool IntentoryItemElement::isValid() const {
	return item != nullptr;
}

InventoryItem* IntentoryItemElement::getItem() const {
	return item;
}
