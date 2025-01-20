#ifndef FRUIT_H
#define FRUIT_H

#include <glm/glm.hpp>
#include "snake.h"

void initFruit();
glm::vec3 generateRandomPosition();
void checkCollision();
void renderFruit(unsigned int shaderProgram);

#endif // FRUIT_H
