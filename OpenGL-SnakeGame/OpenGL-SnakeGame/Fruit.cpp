#include "Fruit.h"

glm::vec2 fruitPos = glm::vec2(0.0f, 0.0f);

void Fruit::spawnNewFruit(Grid g)
{
    int gridX = rand() % g.GRID_SIZE;
    int gridY = rand() % g.GRID_SIZE;
    fruitPos = glm::vec2(
        -g.BORDER_OFFSET + (gridX + 0.5f) * g.GRID_STEP,
        -g.BORDER_OFFSET + (gridY + 0.5f) * g.GRID_STEP
    );
}
