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

bool Snake::CollidesWithFruit()
{
	// Check for collision with the fruit
	if (abs(snake.posX - fruitX) < Grid::GRID_STEP / 2 && abs(snake.posY - fruitY) < Grid::GRID_STEP / 2)
	{
		score++;
		// Move the fruit to a new random position within the grid
		int gridX = rand() % Grid::GRID_SIZE;
		int gridY = rand() % Grid::GRID_SIZE;
		fruitX = -Grid::BORDER_OFFSET + (gridX + 0.5f) * Grid::GRID_STEP;
		fruitY = -Grid::BORDER_OFFSET + (gridY + 0.5f) * Grid::GRID_STEP;
	}
	return false;
}