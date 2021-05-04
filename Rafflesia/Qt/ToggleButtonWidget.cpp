#include "ToggleButtonWidget.h"

#include <QIcon>

ToggleButtonWidget::ToggleButtonWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.button, &QPushButton::clicked, this, &ToggleButtonWidget::onButtonClicked);
}

ToggleButtonWidget::~ToggleButtonWidget()
{
}

void ToggleButtonWidget::init(QString defaultIcon, QString toggledIcon) {
	defaultPixmap = QPixmap(defaultIcon);
	toggledPixmap = QPixmap(toggledIcon);
	ui.button->setIcon(QIcon(defaultPixmap));
}

void ToggleButtonWidget::onButtonClicked() {
	toggled = !toggled;
	ui.button->setIcon(toggled ? QIcon(toggledPixmap): QIcon(defaultPixmap));
	onToggle();
}