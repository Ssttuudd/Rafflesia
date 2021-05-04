#include "CharListElement.h"

#include "Game/Character.h"
#include "QtUtils.h"

QColor CharListElement::selectedColor = Qt::lightGray;
QColor CharListElement::unSelectedColor = Qt::white;

CharListElement::CharListElement(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.hpBar->setStyleSheet("QProgressBar { color: black; } QProgressBar::chunk{background-color:#FF6014}");
	ui.mpBar->setStyleSheet("QProgressBar { color: black; } QProgressBar::chunk{background-color:#4774d6}");
	ui.familyFilteredLabel->setVisible(false);
	ui.mobFilteredLabel->setVisible(false);
}

CharListElement::~CharListElement()
{

}

void CharListElement::setInfos(Character* _character) {
	character = _character;
	objectId = character->getId();
	ui.title->setText(character->getName().c_str());
	updateBarFlatValue(ui.hpBar, character->getHp(), character->getMaxHp());
	updateBarFlatValue(ui.mpBar, character->getMp(), character->getMaxMp());
}

uint32_t CharListElement::getId() {
	return objectId;
}

bool CharListElement::isValid() const {
	return character != nullptr;
}

const float CharListElement::getDistance() const {
	return distance;
}

void CharListElement::setInfo(float value, bool filtered, bool familyFiltered) {
	distance = value;
	ui.mobFilteredLabel->setVisible(filtered);
	ui.familyFilteredLabel->setVisible(familyFiltered);
}

Position CharListElement::getPosition() const {
	if (character)
		return character->getPosition();

	return { 0, 0, 0 };
}

void CharListElement::setTargeted(bool value) {
	QPalette pal = palette();
	pal.setColor(QPalette::Base, value ? selectedColor : unSelectedColor);
	setAutoFillBackground(true);
	setPalette(pal);
}

Character* CharListElement::getCharacter() const {
	return character;
}