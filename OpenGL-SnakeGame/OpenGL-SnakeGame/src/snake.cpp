#include "snake.h"
#include <glm/glm.hpp>
#include <vector>
#include <GLFW/glfw3.h>

std::vector<SnakeSegment> snake;

void initSnake() {
    snake.push_back({ glm::vec3(0.0f, 0.0f, 0.0f) }); // Initialize snake with a single segment
}

void updateSnake(const glm::vec3& direction) {
    glm::vec3 newHead = snake.front().position + direction;
    snake.insert(snake.begin(), { newHead }); // Add new head
    snake.pop_back(); // Remove the tail
}

void renderSnake(glm::vec3 position, unsigned int shaderProgram) {
    for (const auto& segment : snake) {
        renderCube(position, shaderProgram);
    }
}
