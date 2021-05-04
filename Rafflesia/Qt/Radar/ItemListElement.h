#pragma once

#include <QWidget>
#include "ui_ItemListElement.h"

#include "ActorListWidgetItem.hpp"
#include "Game/Game.h"

class DroppedItem;
class ItemListElement : public QWidget
{
	Q_OBJECT

public:
	ItemListElement(QWidget *parent = Q_NULLPTR);
	~ItemListElement();

	void setInfos(DroppedItem* character, bool filtered);
	void setInfo(float value, bool filtered);
	uint32_t getId();
	bool isValid() const;
	Position getPosition() const;
	const float getDistance() const;
	DroppedItem* getItem() const;

private:
	Ui::ItemListElement ui;
	DroppedItem* item = nullptr;
	uint32_t objectId = 0;
	float distance = 99999999999;
};
