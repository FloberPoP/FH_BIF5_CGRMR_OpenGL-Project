#pragma once
#include "Grid.h"
#include "glm/glm.hpp"
#include <cmath>

class Fruit
{
public: 
	glm::vec2 fruitPos = glm::vec2(0.0f, 0.0f);
	void spawnNewFruit(Grid g);
};

