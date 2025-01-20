#include "fruit.h"
#include "snake.h"
#include "rendering.h"
#include <cstdlib>

glm::vec3 fruitPosition;
bool fruitEaten;

void initFruit() {
    fruitEaten = false;
    fruitPosition = generateRandomPosition();
}

glm::vec3 generateRandomPosition() {
    int x = rand() % 20 - 10;
    int y = rand() % 20 - 10;
    return glm::vec3(x, y, 0.0f);
}

void checkCollision() {
    if (glm::distance(snake.front().position, fruitPosition) < 0.5f) {
        fruitEaten = true;
        snake.push_back({ snake.back().position }); // Grow the snake
        fruitPosition = generateRandomPosition();  // Respawn fruit
    }
}

void renderFruit(unsigned int shaderProgram) {
    renderCube(fruitPosition, shaderProgram); // Render the fruit cube
}
