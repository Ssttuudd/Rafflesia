#pragma once

#include <QWidget>
#include "ui_CharListElement.h"

#include "ActorListWidgetItem.hpp"
#include "Game/Game.h"

class Character;
class CharListElement : public QWidget
{
	Q_OBJECT

public:
	CharListElement(QWidget *parent = Q_NULLPTR);
	~CharListElement();
	
	void setInfos(Character* character);
	uint32_t getId();
	Character* getCharacter() const;
	void setInfo(float value, bool filtered, bool familyFiltered);
	Position getPosition() const;
	bool isValid() const;
	void setTargeted(bool value);

	const float getDistance() const;
private:
	Ui::CharListElement ui;
	uint32_t objectId = 0;
	Character* character = nullptr;

	static QColor selectedColor;
	static QColor unSelectedColor;

	float distance = 99999999999;
};
