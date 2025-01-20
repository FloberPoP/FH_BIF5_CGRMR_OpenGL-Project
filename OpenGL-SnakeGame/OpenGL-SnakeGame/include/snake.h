#ifndef SNAKE_H
#define SNAKE_H

#include <glm/glm.hpp>
#include <vector>

// Snake segment structure
struct SnakeSegment {
    glm::vec3 position;
};

// Declare the snake variable as extern
extern std::vector<SnakeSegment> snake;

// Functions for snake logic
void initSnake();
void updateSnake(const glm::vec3& direction);
void renderSnake(glm::vec3 position, unsigned int shaderProgram);
void renderCube(glm::vec3 position, unsigned int shaderProgram); // Utility function for rendering a cube

#endif // SNAKE_H
