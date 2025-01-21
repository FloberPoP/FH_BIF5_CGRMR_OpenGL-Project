#include "Snake.h"

Snake::Snake(float gridFieldSize)
{
	_gridFieldSize = gridFieldSize;
	posX = 0.0f + gridFieldSize / 2;
	posY = 0.0f + gridFieldSize / 2;
}

void Snake::ApplyMovement()
{
	posX += dirX * _gridFieldSize;
	posY += dirY * _gridFieldSize;
}