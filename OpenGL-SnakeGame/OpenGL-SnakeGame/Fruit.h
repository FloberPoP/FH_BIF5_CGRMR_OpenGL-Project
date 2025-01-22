#pragma once
#include "Grid.h"
#include "glm/glm.hpp"
#include <cmath>

class Fruit
{
public:
	Fruit();
	unsigned int fruitID = 0;
	void SetRandomPos();

	glm::vec2 GetPos() { return _pos; }

private:
	glm::vec2 _pos = glm::vec2(0.0f, 0.0f);
	int fruitcount = 1;
	Fruit(const int score);
	void SetRandomPos();
	void RandomFruit();
};

