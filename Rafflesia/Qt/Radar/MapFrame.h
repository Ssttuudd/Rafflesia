#pragma once

#include "Game/Game.h"

#include <QFrame>

#include "Bot/Bot.h"

class Game;

class MapFrame : public QFrame
{
	Q_OBJECT

public:
	MapFrame(QWidget *parent = Q_NULLPTR);
	~MapFrame();

	void init(Game* game, Bot* bot);
	void reset();

protected:
	void paintEvent(QPaintEvent*) override;
	bool event(QEvent* event) override;
	void mousePressEvent(QMouseEvent* ) override; 
	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void wheelEvent(QWheelEvent* ) override;

	QPoint gameToMap(Position pos) const;
	Position mapToGame(QPoint pos) const;

	void LoadMapData(Position p);
	void DrawMap(QPainter* painter, Position p);
private:
	Game* game = nullptr;
	Bot* bot = nullptr;
	QPixmap* mapPixmap = nullptr;

	PolygonPosition selectedPosition;
	Position playerPosition;
	int zoom = 50;
	int widgetMidWidth = 0;
	int widgetMidHeight = 0;
	bool needreset = false;

	PolygonPosition findClosePolygonPosition(const QPoint& position, float distance) const;
	const Character* getCloseCharacter(const Position& position) const;
	void drawEntity(QPainter& painter, QPoint position, QColor penColor, QColor brushColor, int size);
};
