#pragma once
#include <glm/glm.hpp>
#include "Grid.h"

class Snake
{
public:
	Snake(float gridFieldSize);
	void ApplyMovement();
	bool CollidesWithBorder();
	bool CollidesWithSelf();
	bool CollidesWithFruit();

	glm::vec2 GetPos() { return _pos; }

	glm::vec2 dir;

private:
	float _gridFieldSize;
	glm::vec2 _pos;
};

