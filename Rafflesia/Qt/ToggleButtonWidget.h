#pragma once

#include <QWidget>
#include "ui_ToggleButtonWidget.h"

class ToggleButtonWidget : public QWidget
{
	Q_OBJECT

public:
	ToggleButtonWidget(QWidget *parent = Q_NULLPTR);
	~ToggleButtonWidget();
	void init(QString defaultIcon, QString toggledIcon);
	bool isToggled() { return toggled; }

public slots:
	void onButtonClicked();
signals:
	void onToggle();

private:
	Ui::ToggleButtonWidget ui;
	QPixmap defaultPixmap;
	QPixmap toggledPixmap;
	bool toggled = false;
};
