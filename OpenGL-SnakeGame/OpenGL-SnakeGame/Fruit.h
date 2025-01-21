#pragma once
#include "Grid.h"
class Fruit
{
public: 
	glm::vec2 fruitPos = glm::vec2(0.0f, 0.0f);
	void spawnNewFruit(Grid g);
};

