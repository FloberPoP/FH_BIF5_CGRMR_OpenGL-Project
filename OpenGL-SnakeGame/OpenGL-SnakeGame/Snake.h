#pragma once
#include <glm/glm.hpp>
#include "Grid.h"
#include "Fruit.h"
#include "SnakePart.h"

class Snake
{
public:
	Snake();
	~Snake();
	void ApplyMovement();
	bool CollidesWithBorder();
	bool CollidesWithSelf();
	bool CollidesWithFruit(Fruit fruit);

	void Grow();


	const SnakePart* GetTail() const { return tail; }
	const SnakePart& GetHead() const { return *head; }
	const glm::vec2 GetPos() const { return head->pos; }
	const glm::vec2 GetTailDir() const { return (tail->prev) ? glm::normalize(tail->prev->pos - tail->pos) : dir; }

	glm::vec2 dir;

private:
	SnakePart* head;
	SnakePart* tail;
};

