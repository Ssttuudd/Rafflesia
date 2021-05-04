#include "RadarWidget.h"

#include "RadarWidget.h"

#include <QPainter.h>
#include <QEvent.h>

#include "Bot/Bot.h"
#include "Game/Npc.h"

#define ZOOM_MIN 10
#define ZOOM_MAX 100
#define MOUSE_WHEEL_SENSITIVITY 50
#define POLYGON_POINT_REMOVE_DIST 20
#define POLYGON_POINT_MOVE_DIST 20

#define POLYGON_SIZE 2
#define POLYGON_COLOR "#FF6347"
#define ITEM_SIZE 2
#define ITEM_COLOR "#FF6347"

#define NPC_SIZE 7
#define MOB_SIZE 8
#define PLAYER_SIZE 9
#define LOCAL_PLAYER_SIZE 2
#define TILE_SIZE 900
#define GAME_TILE_LOC_SIZE (1 << 15)


RadarWidget::RadarWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.clearPolygonButton, &QPushButton::clicked, this, &RadarWidget::onClearButtonClicked);
	connect(ui.addPolygonButton, &QPushButton::clicked, this, &RadarWidget::onAddPolygonButtonClicked);
	connect(ui.polygonTypeToggle, &ToggleButtonWidget::onToggle, this, &RadarWidget::onTogglePolygonType);

	ui.polygonTypeToggle->init(":/icons/polygonInclude.png", ":/icons/polygonExclude.png");
}

RadarWidget::~RadarWidget()
{
}

void RadarWidget::init(Game* _game, Bot* _bot) {
	game = _game;
	bot = _bot;
	ui.mapWidget->init(_game, _bot);
}

void RadarWidget::onClearButtonClicked() {
	bot->resetPolygons();
}

void RadarWidget::onAddPolygonButtonClicked() {
	bot->addPolygon(polygonType);
}

void RadarWidget::onTogglePolygonType() {
	polygonType = (polygonType == EPolygon::Include) ? EPolygon::Exclude : EPolygon::Include;
	bot->addPolygon(polygonType);
}

void RadarWidget::resetMap() {
	ui.mapWidget->reset();
}