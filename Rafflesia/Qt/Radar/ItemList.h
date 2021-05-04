#pragma once

#include <QWidget>
#include "ui_ItemList.h"

class Bot;
class CharListElement;
class Game;
class DroppedItem;
class ItemList : public QWidget
{
	Q_OBJECT

public:
	ItemList(QWidget *parent = Q_NULLPTR);
	~ItemList();

	void init(Game* game, Bot* _bot);
	void update(float dt);
	void addItem(DroppedItem* item);
	void updateItem(DroppedItem* item);
	void removeItem(uint32_t objectId);

private:
	Ui::ItemList ui;
	Bot* bot = nullptr;
	Game* game = nullptr;
	uint32_t currentTargetId = 0;
	int sortRole = Qt::UserRole + 1;
	DroppedItem* rightClickedItem = nullptr;

	QListWidgetItem* getRowItem(uint32_t objectId);
	void removeListItem(QListWidgetItem* item);
	void onItemClicked(QListWidgetItem* item);

signals:
	void onItemSelected(DroppedItem* item);
	void ignoreItem(uint32_t id);
private slots:
	void showContextMenu(const QPoint& pos);
	void onIgnoreItem();
};
