#include "MapPolygon.h"

#include <algorithm>

void MapPolygon::setType(EPolygon type) {
	polygonType = type;
}

EPolygon MapPolygon::getType() const {
	return polygonType;
}

void MapPolygon::addPositions(std::vector<Position> _positions) {
	positions = _positions;
}

void MapPolygon::addPosition(Position _position) {
	positions.push_back(_position);
}

void MapPolygon::removePosition(int positionId) {
	positions.erase(positions.begin() + positionId);
}

void MapPolygon::movePosition(uint32_t positionId, Position _position) {
	if (positionId > 0 && positionId < positions.size()) {
		positions[positionId] = _position;
	}
}

bool MapPolygon::isInside(Position p) const
{
	auto N = positions.size();
	if (N < 3) {
		return false;
	}
	int counter = 0;
	int i;
	double xinters;
	Position p1, p2;

	p1 = positions[0];
	for (i = 1; i <= N; i++) {
		p2 = positions[i % N];
		if (p.y > std::min(p1.y, p2.y)) {
			if (p.y <= std::max(p1.y, p2.y)) {
				if (p.x <= std::max(p1.x, p2.x)) {
					if (p1.y != p2.y) {
						xinters = (p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + (double)p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	return counter % 2 != 0;
}

const std::vector<Position>& MapPolygon::getPositions() const {
	return positions;
}

size_t MapPolygon::size() const {
	return positions.size();
}