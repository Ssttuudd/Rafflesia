#pragma once

#include <vector>

#include "Game/GameData.h"

enum class EPolygon {
	Include,
	Exclude
};

class MapPolygon
{
public:
	void setType(EPolygon type);
	EPolygon getType()const;
	void addPositions(std::vector<Position> _positions);
	void addPosition(Position _position);
	void removePosition(int positionId);
	void movePosition(uint32_t positionId, Position _position);
	bool isInside(Position p) const;
	const std::vector<Position>& getPositions() const;
	size_t size() const;

private:
	std::vector<Position> positions;
	EPolygon polygonType;
};


