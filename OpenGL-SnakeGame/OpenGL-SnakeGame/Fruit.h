#pragma once
#include "Grid.h"
#include "glm/glm.hpp"
#include <cmath>
enum FruitE {
	Cherry = 0,
	Banana = 1
};
class Fruit
{
public:
	Fruit();
	glm::vec2 GetPos() { return _pos; }
	FruitE fruitType;

private:
	glm::vec2 _pos = glm::vec2(0.0f, 0.0f);
	void SetRandomPos();
};

