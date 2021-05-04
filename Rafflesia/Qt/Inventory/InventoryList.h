#pragma once

#include <QWidget>
#include "ui_InventoryList.h"

#include "Inventory/IntentoryItemElement.h"

class InventoryList : public QWidget
{
	Q_OBJECT

public:
	InventoryList(QWidget *parent = Q_NULLPTR);
	~InventoryList();

	void addItem(InventoryItem* item);
	void removeItem(uint32_t objectId);
	void removeListItem(QListWidgetItem* item);
	void updateItem(InventoryItem* item);

private:
	Ui::InventoryWidget ui;

	QListWidgetItem* getRowItem(uint32_t objectId);
};
