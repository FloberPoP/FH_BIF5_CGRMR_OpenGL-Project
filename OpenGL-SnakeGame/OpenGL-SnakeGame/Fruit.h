#pragma once
#include "Grid.h"
#include "glm/glm.hpp"
#include <cmath>

class Fruit
{
public:
	Fruit();

	// ?
	void spawnNewFruit(Grid g);

	glm::vec2 GetPos() { return _pos; }

private:
	glm::vec2 _pos = glm::vec2(0.0f, 0.0f);

	void SetRandomPos();
};

