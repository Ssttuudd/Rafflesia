#pragma once

#include <QWidget>
#include "ui_IntentoryItemElement.h"

#include "Game/InventoryItem.h"

class IntentoryItemElement : public QWidget
{
	Q_OBJECT

public:
	IntentoryItemElement(QWidget *parent = Q_NULLPTR);
	~IntentoryItemElement();

	void setInfos(InventoryItem* character);
	uint32_t getId();
	bool isValid() const;
	InventoryItem* getItem() const;

private:
	Ui::IntentoryItemElement ui;
	InventoryItem* item = nullptr;
	uint32_t objectId = 0;
};
