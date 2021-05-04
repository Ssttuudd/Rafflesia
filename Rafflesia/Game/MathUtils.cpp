#include "MathUtils.h"

#include <algorithm>    // std::max/min 
#include <cmath>

float distance(Position pos1, Position pos2) {
	return distance(pos1.x, pos1.y, pos2.x, pos2.y);
}

float distance(int x1, int y1, int x2, int y2) {
	return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

float distance(float z1, float z2) {
	return std::abs(std::abs(z1) - std::abs(z2));
}