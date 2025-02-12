#include "direction.h"

direction turnto(direction d, direction t) {
	if(d == Center || t == Center)
		return Center;
	// Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
	static direction matrix[8][8] = {
		{Center, Right, Right, Right, Right, Left, Left, Left}, // Up
		{Left, Center, Right, Right, Right, Right, Left, Left}, // RightUp
		{Left, Left, Center, Right, Right, Right, Right, Left}, // Right
		{Left, Left, Left, Center, Right, Right, Right, Right}, // RightDown
		{Right, Left, Left, Left, Center, Right, Right, Right}, // Down
		{Right, Right, Left, Left, Left, Center, Right, Right}, // LeftDown
		{Right, Right, Right, Left, Left, Left, Center, Right}, // Left
		{Right, Right, Right, Right, Left, Left, Left, Center}, // LeftUp
	};
	return matrix[d - 1][t - 1];
}

bool isdiagonal(direction d) {
	switch(d) {
	case RightUp: case RightDown: case LeftDown: case LeftUp: return true;
	default: return false;
	}
}

point getpoint(direction d) {
	switch(d) {
	case Left: return {-1, 0};
	case LeftUp: return {-1, -1};
	case LeftDown: return {-1, 1};
	case Right: return {1, 0};
	case RightUp: return {1, -1};
	case RightDown: return {1, 1};
	case Up: return {0, -1};
	case Down: return {0, 1};
	default: return {0, 0};
	}
}

direction to(direction d, direction s) {
	switch(s) {
	case Left:
		switch(d) {
		case Left: return LeftDown;
		case LeftUp: return Left;
		case Up: return LeftUp;
		case RightUp: return Up;
		case Right: return RightUp;
		case RightDown: return Right;
		case Down: return RightDown;
		case LeftDown: return Down;
		default: return Center;
		}
	case Right:
		switch(d) {
		case Left: return LeftUp;
		case LeftUp: return Up;
		case Up: return RightUp;
		case RightUp: return Right;
		case Right: return RightDown;
		case RightDown: return Down;
		case Down: return LeftDown;
		case LeftDown: return Left;
		default: return Center;
		}
	case Down:
		switch(d) {
		case Left: return Right;
		case LeftUp: return RightDown;
		case Up: return Down;
		case RightUp: return LeftDown;
		case Right: return Left;
		case RightDown: return LeftUp;
		case Down: return Up;
		case LeftDown: return RightUp;
		default: return Center;
		}
	default:
		return d;
	}
}