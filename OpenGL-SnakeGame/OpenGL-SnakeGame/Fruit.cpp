#include "Fruit.h"

glm::vec2 fruitPos = glm::vec2(0.0f, 0.0f);

Fruit::Fruit()
{
    fruitType = FruitE(rand() % 2);
    SetRandomPos();
}

void Fruit::SetRandomPos()
{
    int gridX = rand() % Grid::GRID_SIZE;
    int gridY = rand() % Grid::GRID_SIZE;
    _pos = glm::vec2(
        -Grid::BORDER_OFFSET + (gridX + 0.5f) * Grid::GRID_STEP,
        -Grid::BORDER_OFFSET + (gridY + 0.5f) * Grid::GRID_STEP
    );
}



