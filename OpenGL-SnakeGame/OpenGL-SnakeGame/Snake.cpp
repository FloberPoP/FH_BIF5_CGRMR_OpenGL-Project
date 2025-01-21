#include "Snake.h"

Snake::Snake()
{
	tail = new SnakePart(glm::vec2(Grid::GRID_STEP / 2, Grid::GRID_STEP / 2));
	head = tail;
}

void Snake::ApplyMovement()
{
	if (tail->prev != nullptr && tail != head) // Snake length > 1
	{
		tail->pos = head->pos;
		head->prev = tail;
		tail = tail->prev;
		head = head->prev;
		head->prev = nullptr;
	}

	head->pos.x += dir.x * Grid::GRID_STEP;
	head->pos.y += dir.y * Grid::GRID_STEP;
}

bool Snake::CollidesWithBorder()
{
	// Check for border collision
	if (GetPos().x < -Grid::BORDER_OFFSET || GetPos().x > Grid::BORDER_OFFSET || GetPos().y < -Grid::BORDER_OFFSET || GetPos().y > Grid::BORDER_OFFSET)
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
	if (abs(GetPos().x - fruit.GetPos().x) < Grid::GRID_STEP / 2 && abs(GetPos().y - fruit.GetPos().y) < Grid::GRID_STEP / 2)
	{
		return true;
	}
	return false;
}

void Snake::Grow()
{
	tail = new SnakePart(tail->pos, tail);
}