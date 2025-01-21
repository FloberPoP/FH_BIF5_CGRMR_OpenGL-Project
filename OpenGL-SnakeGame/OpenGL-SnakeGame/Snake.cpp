#include "Snake.h"

Snake::Snake(float gridFieldSize)
{
	_gridFieldSize = gridFieldSize;
	_pos = glm::vec2(gridFieldSize / 2, gridFieldSize / 2);
}

void Snake::ApplyMovement()
{
	_pos.x += dir.x * _gridFieldSize;
	_pos.y += dir.y * _gridFieldSize;
}

bool Snake::CollidesWithBorder()
{
	// Check for border collision
	if (_pos.x < -Grid::BORDER_OFFSET || _pos.x > Grid::BORDER_OFFSET || _pos.y < -Grid::BORDER_OFFSET || _pos.y > Grid::BORDER_OFFSET)
	{
		return true;
	}
	return false;
}

bool Snake::CollidesWithSelf()
{
	return false;
}

bool Snake::CollidesWithFruit(Fruit fruit)
{
	// Check for collision with the fruit
	if (abs(_pos.x - fruit.fruitPos.x) < Grid::GRID_STEP / 2 && abs(_pos.y - fruit.fruitPos.y) < Grid::GRID_STEP / 2)
	{
		return true;
	}
	return false;
}