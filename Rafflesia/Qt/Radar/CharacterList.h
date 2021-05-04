#pragma once

#include <QWidget>
#include "ui_CharacterList.h"

class Bot;
class Character;
class CharListElement;
class Game;
class CharacterList : public QWidget
{
	Q_OBJECT

public:
	CharacterList(QWidget *parent = Q_NULLPTR);
	~CharacterList();

	void init(Game* game, Bot* _bot);
	void update(float dt);
	void addCharacter(Character* character);
	void updateCharacter(Character* character);
	void removeCharacter(uint32_t objectId);
	void targetCharacter(Character* target);
	void removeCurrentTarget();

private:
	Ui::CharacterList ui;
	Bot* bot = nullptr;
	Game* game = nullptr;
	uint32_t currentTargetId = 0;
	int sortRole = Qt::UserRole + 1;
	Character* rightClickedCharacter = nullptr;

	QListWidgetItem* getRowItem(uint32_t objectId);
	void removeListItem(QListWidgetItem* item);
	void onItemClicked(QListWidgetItem* item);

signals:
	void onCharacterSelected(Character* character);
	void ignoreMobFamily(uint32_t id);
	void ignoreMob(uint32_t id);
private slots:
	void showContextMenu(const QPoint& pos);
	void onIgnoreMobFamilyAction();
	void onIgnoreMobAction();
};
