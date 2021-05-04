#pragma once

#include <QWidget>
#include "ui_RadarWidget.h"

#include "Bot/Bot.h"

class RadarWidget : public QWidget
{
	Q_OBJECT

public:
	RadarWidget(QWidget *parent = Q_NULLPTR);
	~RadarWidget();

	void init(Game* game, Bot* bot);
	void resetMap();
protected slots:
	void onClearButtonClicked();
	void onAddPolygonButtonClicked();
	void onTogglePolygonType();
	
private:
	Ui::RadarWidget ui;
	Game* game = nullptr;
	Bot* bot = nullptr;

	EPolygon polygonType = EPolygon::Include;
};
