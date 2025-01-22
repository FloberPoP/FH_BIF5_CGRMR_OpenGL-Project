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


	SnakePart* GetTail() { return tail; }
	SnakePart& GetHead() { return *head; }
	glm::vec2 GetPos() { return head->pos; }

	glm::vec2 dir;

private:
	SnakePart* head;
	SnakePart* tail;
};

