#pragma once
#include "Grid.h"
#include "glm/glm.hpp"
#include <cmath>
enum FruitE {
	Cherry = 0,
	Banana = 1,
	Meat = 2,
	Eggplant = 3,
	Beer = 4,
	Orange = 5,
	Pineapple = 6,
	Peach = 7,
	Brocoli = 8,
	Pear = 9
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

