#pragma once

#include <QWidget>
#include "ui_QtCondition.h"

class QtCondition : public QWidget
{
	Q_OBJECT

public:
	QtCondition(QWidget *parent = nullptr);
	~QtCondition();

private:
	Ui::QtConditionClass ui;
};
