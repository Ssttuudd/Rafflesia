#include "MapFrame.h"

#include <QPainter.h>
#include <QEvent.h>
#include <QTooltip>

#include "Bot/Bot.h"
#include "Game/Npc.h"

#define ZOOM_MIN 10
#define ZOOM_MAX 100
#define MOUSE_WHEEL_SENSITIVITY 50
#define POLYGON_POINT_REMOVE_DIST 20
#define POLYGON_POINT_MOVE_DIST 10
#define TOOLTIP_DIST 10

#define POLYGON_SIZE 2
#define POLYGON_INCLUDE_COLOR "#ebe534"
#define POLYGON_EXCLUDE_COLOR "#eb3434"
#define ITEM_SIZE 2
#define ITEM_COLOR "#FF6347"

#define NPC_SIZE 7
#define MOB_SIZE 8
#define PLAYER_SIZE 9
#define LOCAL_PLAYER_SIZE 2
#define TILE_SIZE 900
#define GAME_TILE_LOC_SIZE (1 << 15)

MapFrame::MapFrame(QWidget *parent)
	: QFrame(parent)
{
	// Set background color
	QPalette colors;
	colors.setColor(QPalette::Window, QColor("#99ccff"));
	setAutoFillBackground(true);
	setPalette(colors);
}

void MapFrame::init(Game* _game, Bot* _bot) {
	game = _game;
	bot = _bot;
}

void MapFrame::reset() {
	needreset = true;
}

void MapFrame::LoadMapData(Position p)
{
	// 0 -> 32768, 65536+ = 20_20
	// -32768 -> 0, 65536+ = 19_20
	const int radareZeroX = 20;
	const int radareZeroY = 18;
	int RadareX = radareZeroX + p.x / GAME_TILE_LOC_SIZE;
	int RadareY = radareZeroY + p.y / GAME_TILE_LOC_SIZE;
	/*const int radareZeroX = 2;
	const int radareZeroY = 1;
	int RadareX = radareZeroX + (p.x + GAME_TILE_LOC_SIZE / 2) / GAME_TILE_LOC_SIZE;
	int RadareY = radareZeroY + (p.y + GAME_TILE_LOC_SIZE / 2 ) / GAME_TILE_LOC_SIZE;*/

	// Put NTILES*NTILES tiles in cache (the higher, the more memory usage)
	const int NTILES = 3;
	mapPixmap = new QPixmap(TILE_SIZE * NTILES, TILE_SIZE * NTILES);
	QPainter cachePainter(mapPixmap);
	auto startx = RadareX - NTILES / 2;
	auto starty = RadareY - NTILES / 2;
	for (auto sy = 0; sy < NTILES; sy++) {
		for (auto sx = 0; sx < NTILES; sx++) {
			uint32_t ox = startx + sx;
			uint32_t oy = starty + sy;
			if (ox < 0 || oy < 0) {
				continue;
			}

			QPixmap pixmap;
			QString mapName("../Rafflesia/Data/Maps/%1_%2.jpg");
			mapName = mapName.arg(ox).arg(oy);
			pixmap.load(mapName);
			cachePainter.drawPixmap(sx * TILE_SIZE, sy * TILE_SIZE, pixmap.width(), pixmap.height(), pixmap);
		}
	}
}

void MapFrame::DrawMap(QPainter* painter, Position p)
{
	if (!mapPixmap || needreset) {
		LoadMapData(p);
		needreset = false;
	}
	qreal const pixel_ratio = ((qreal) GAME_TILE_LOC_SIZE / (qreal) TILE_SIZE);
	qreal z = zoom;

	/// Let's say it's a problem in a 2d space where
	/// P is player clickPosition
	/// V is viewport clickPosition (top left)
	/// M is map clickPosition (top left)
	///
	///
	///    M
	///     +------+------+------+
	///     |      |      |      |
	///     |  V___|______|______|____
	///     +---|--+------+------+   |
	///     |   |  |      |      |   |
	///     |   |  |      P      |   |
	///     +---|--+------+------+   |
	///     |   |__|______|______|___|
	///     |      |      |      |
	///     +------+------+------+
	///
	/// The map is positioned relatively to the top left of the viewport (painter->drawPixmap)
	/// So we need to compute VM s.t. VM = VP + PM
	/// P is always in the middle of the viewport, so VP is trivial to compute
	/// PM can be computed using the game coordinates
	
	auto locx_pixel = (p.x % GAME_TILE_LOC_SIZE) / pixel_ratio;
	auto locy_pixel = (p.y % GAME_TILE_LOC_SIZE) / pixel_ratio;
	auto PMx = - TILE_SIZE - locx_pixel;
	auto PMy = - TILE_SIZE - locy_pixel;
	PMx /= (z / 50);
	PMy /= (z / 50);
	auto VPx = width() / 2;
	auto VPy = height() / 2;
	
	auto VMx = PMx + VPx;
	auto VMy = PMy + VPy;
	auto w = mapPixmap->width() / (z / 50);
	auto h = mapPixmap->height() / (z / 50);

	painter->drawPixmap(VMx, VMy, w, h, *mapPixmap);
}

QPoint MapFrame::gameToMap(Position position) const {
	auto entityMapX = widgetMidWidth + (position.x - playerPosition.x) / zoom;
	auto entityMapY = widgetMidHeight + (position.y - playerPosition.y) / zoom;
	return QPoint(entityMapX, entityMapY);
}

Position MapFrame::mapToGame(QPoint position) const {

	auto x = zoom * (position.x() - widgetMidWidth) + playerPosition.x;
	auto y = zoom * (position.y() - widgetMidHeight) + playerPosition.y;
	return { x, y };
}

void MapFrame::drawEntity(QPainter& painter, QPoint position, QColor penColor, QColor brushColor, int size) {
	painter.setPen(QPen(penColor, 2));
	brushColor.setAlpha(64);
	painter.setBrush(QBrush(brushColor));
	painter.drawEllipse(position.x(), position.y(), size, size);
}


bool MapFrame::event(QEvent* event)
{
	if (event->type() == QEvent::ToolTip) {
		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
		const auto& pos = helpEvent->pos();
		auto closeChar = getCloseCharacter({ pos.x(), pos.y() });
		if (closeChar) {
			QToolTip::showText(helpEvent->globalPos(), closeChar->getName().c_str());
		}
		else {
			QToolTip::hideText();
			event->ignore();
		}

		return true;
	}
	return QWidget::event(event);
}

void MapFrame::paintEvent(QPaintEvent*)
{
	update();
	auto player = game->getPlayer();
	if (!player) {
		return;
	}

	playerPosition = player->getPosition();
	widgetMidWidth = width() / 2;
	widgetMidHeight = height() / 2;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QColor borderColor = Qt::white;
	QColor fillColor = Qt::white;

	// First map
	DrawMap(&painter, player->getPosition());

	// Second polygon
	const auto& polygons = bot->getPolygons();
	for (const auto& polygon : polygons) {
		std::vector<QPoint> points;
		const auto& positions = polygon.getPositions();
		auto color = polygon.getType() == EPolygon::Include ? QColor(POLYGON_INCLUDE_COLOR) : QColor(POLYGON_EXCLUDE_COLOR);
		painter.setBrush(QBrush(color));
		painter.setPen(QPen(color, 2));
		for (const auto& point : positions) {
			auto pointPos = gameToMap(point);
			painter.drawEllipse(pointPos.x(), pointPos.y(), POLYGON_SIZE, POLYGON_SIZE);
			points.push_back(pointPos);
		}

		color.setAlpha(64);
		painter.setBrush(QBrush(color));
		painter.drawPolygon(points.data(), points.size());
	}

	auto droppedItems = game->getItems();
	for (const auto& item : droppedItems) {
		painter.setPen(QPen(QColor(ITEM_COLOR), 5));
		auto itemPos = gameToMap(item.second->getPosition());
		painter.drawEllipse(itemPos.x(), itemPos.y(), ITEM_SIZE, ITEM_SIZE);
	}

	// Third entities
	auto gameEntities = game->getCharacters();
	for (const auto& entityPair : gameEntities) {
		auto entity = entityPair.second;
		if (entity == player) {
			continue;
		}

		auto entityPos = gameToMap(entity->getPosition());
		auto size = 0;

		if (entity->isPlayer()) {
			borderColor = QColor("#FF5D00");
			size = PLAYER_SIZE;
			entityPos.setX(entityPos.x() - PLAYER_SIZE / 2);
			entityPos.setY(entityPos.y() - PLAYER_SIZE / 2);
		}
		else {
			if (entity->isAttackable()) {
				borderColor = QColor("#0021B6");
				size = MOB_SIZE;
				entityPos.setX(entityPos.x() - MOB_SIZE / 2);
				entityPos.setY(entityPos.y() - MOB_SIZE / 2);
			}
			else {
				borderColor = QColor("#E42194");
				size = NPC_SIZE;
				entityPos.setX(entityPos.x() - NPC_SIZE / 2);
				entityPos.setY(entityPos.y() - NPC_SIZE / 2);
			}
		}
		fillColor = borderColor;

		if (entity.get() == player->getTarget() && entity->isNpc()) {
			fillColor = QColor("#e342f5");
		}
		if (entity->getTarget() == player.get()) {
			borderColor = QColor("#e342f5");
		}
		drawEntity(painter, entityPos, borderColor, fillColor, size);
	}

	// Finaly local player
	painter.setPen(QPen(Qt::white, 1));
	QPoint topLeft(widgetMidWidth - LOCAL_PLAYER_SIZE, widgetMidHeight - LOCAL_PLAYER_SIZE);
	QPoint bottomRight(widgetMidWidth + LOCAL_PLAYER_SIZE, widgetMidHeight + LOCAL_PLAYER_SIZE);
	painter.drawRect(QRect(topLeft, bottomRight));
}

void MapFrame::wheelEvent(QWheelEvent* qEvent) {
	zoom += qEvent->angleDelta().y() / MOUSE_WHEEL_SENSITIVITY;
	zoom = std::min(std::max(zoom, ZOOM_MIN), ZOOM_MAX);
}

void MapFrame::mouseMoveEvent(QMouseEvent* qEvent) {
	QFrame::mouseMoveEvent(qEvent);
	if (selectedPosition.isValid()) {
		bot->movePolygonPoint(selectedPosition, mapToGame({ qEvent->x(), qEvent->y() }));
	}
}

void MapFrame::mouseReleaseEvent(QMouseEvent* qEvent) {
	QFrame::mouseReleaseEvent(qEvent);
	selectedPosition = { -1,-1 };
}

void MapFrame::mousePressEvent(QMouseEvent* qEvent) {
	QFrame::mousePressEvent(qEvent);

	QPoint clickPos{ qEvent->x(), qEvent->y() };

	if (qEvent->button() == Qt::MouseButton::LeftButton) {
		selectedPosition = findClosePolygonPosition(clickPos, POLYGON_POINT_MOVE_DIST);

		if(!selectedPosition.isValid()) {
			bot->addPolygonPoint(mapToGame({ qEvent->x(), qEvent->y() }));
		}
	}
	else if (qEvent->button() == Qt::MouseButton::RightButton) {
		selectedPosition = findClosePolygonPosition(clickPos, POLYGON_POINT_REMOVE_DIST);
		if (selectedPosition.isValid()) {
			bot->removePolygonPosition(selectedPosition);
		}
	}
}

MapFrame::~MapFrame()
{
}

PolygonPosition MapFrame::findClosePolygonPosition(const QPoint& clickPosition, float precision) const {
	const auto& polygons = bot->getPolygons();
	for (int i(0); i < polygons.size(); ++i) {
		const auto positions = polygons[i].getPositions();
		auto result = std::find_if(positions.begin(), positions.end(), [this, clickPosition, precision](const auto& position) {
			const auto mapPos = gameToMap(position);
			return distance(clickPosition.x(), clickPosition.y(), mapPos.x(), mapPos.y()) < precision;
		});
		if (result != std::end(positions)) {
			return { i, (int)(result - positions.begin()) };
		}
	}
	return { -1,-1 };
}

const Character* MapFrame::getCloseCharacter(const Position& position) const {
	const auto entities = game->getCharacters();
	for (const auto charPair : entities) {
		auto mapPos = gameToMap(charPair.second->getPosition());
		if (distance({ mapPos.x(), mapPos.y() }, position) < TOOLTIP_DIST) {
			return charPair.second.get();
		}
	}
	return nullptr;
}