#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "../../vcpkg/packages/glm_x64-windows/include/glm/fwd.hpp"
#include "../Snake.h"

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// Grid properties
const int GRID_SIZE = 30; // Number of fields along one axis
const float GRID_STEP = 1.8f / GRID_SIZE; // Size of each field (normalized coordinates)
const float BORDER_OFFSET = 0.9f; // Border boundary
const float SNAKE_SIZE = GRID_STEP * 0.9f; // Snake size relative to grid field

Snake snake = Snake(GRID_STEP);

int lastDirInput = 0;
float movementCooldown = 0.2f; // in sec
float movementTimer = 0.0f;
float prevTime;
float currentTime;
float deltaTime;

// Counter for collisions
int score = 0;

// Fruit position
float fruitX = -0.85f;
float fruitY = -0.85f;

// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 offset;

void main()
{
    gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
}
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.5, 1.0, 1.0); // Snake color
}
)";

// Function to draw text (for the score)
void renderText(int score)
{
    std::stringstream ss;
    ss << "Score: " << score;
    std::string text = ss.str();

    glfwSetWindowTitle(glfwGetCurrentContext(), text.c_str());
}

// Grid rendering
void drawGrid()
{
    glBegin(GL_LINES);
    glColor3f(0.8f, 0.8f, 0.8f); // Grid color
    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        float coord = -BORDER_OFFSET + i * GRID_STEP;
        // Vertical lines
        glVertex2f(coord, -BORDER_OFFSET);
        glVertex2f(coord, BORDER_OFFSET);
        // Horizontal lines
        glVertex2f(-BORDER_OFFSET, coord);
        glVertex2f(BORDER_OFFSET, coord);
    }
    glEnd();
}

// Border rendering
void drawBorder()
{
    glColor3f(1.0f, 0.0f, 0.0f); // Border color
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-BORDER_OFFSET, -BORDER_OFFSET);
    glVertex2f(BORDER_OFFSET, -BORDER_OFFSET);
    glVertex2f(BORDER_OFFSET, BORDER_OFFSET);
    glVertex2f(-BORDER_OFFSET, BORDER_OFFSET);
    glEnd();
}

// Draw a cube at a specified position
void drawCube(float x, float y, const float* color)
{
    glColor3fv(color);
    glBegin(GL_QUADS);
    glVertex2f(x - SNAKE_SIZE / 2, y - SNAKE_SIZE / 2);
    glVertex2f(x + SNAKE_SIZE / 2, y - SNAKE_SIZE / 2);
    glVertex2f(x + SNAKE_SIZE / 2, y + SNAKE_SIZE / 2);
    glVertex2f(x - SNAKE_SIZE / 2, y + SNAKE_SIZE / 2);
    glEnd();
}

// Process input to change direction
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && lastDirInput != GLFW_KEY_S)
    {
        snake.dirX = 0.0f;
        snake.dirY = 1.0f;
        lastDirInput = GLFW_KEY_W;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && lastDirInput != GLFW_KEY_W)
    {
        snake.dirX = 0.0f;
        snake.dirY = -1.0f;
        lastDirInput = GLFW_KEY_S;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && lastDirInput != GLFW_KEY_D)
    {
        snake.dirX = -1.0f;
        snake.dirY = 0.0f;
        lastDirInput = GLFW_KEY_A;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && lastDirInput != GLFW_KEY_A)
    {
        snake.dirX = 1.0f;
        snake.dirY = 0.0f;
        lastDirInput = GLFW_KEY_D;
    }
}

// Update snake position and ensure it stays within bounds
bool updateSnakePosition()
{
    // Update position
    snake.ApplyMovement();

    // Check for border collision
    if (snake.posX < -BORDER_OFFSET || snake.posX > BORDER_OFFSET || snake.posY < -BORDER_OFFSET || snake.posY > BORDER_OFFSET)
    {
        return false; // End game
    }

    // Check for collision with the fruit
    if (abs(snake.posX - fruitX) < GRID_STEP / 2 && abs(snake.posY - fruitY) < GRID_STEP / 2)
    {
        score++;
        // Move the fruit to a new random position within the grid
        int gridX = rand() % GRID_SIZE;
        int gridY = rand() % GRID_SIZE;
        fruitX = -BORDER_OFFSET + (gridX + 0.5f) * GRID_STEP;
        fruitY = -BORDER_OFFSET + (gridY + 0.5f) * GRID_STEP;
    }

    return true; // Continue game
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snake Game", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize fruit position
    int initialFruitX = rand() % GRID_SIZE;
    int initialFruitY = rand() % GRID_SIZE;
    fruitX = -BORDER_OFFSET + (initialFruitX + 0.5f) * GRID_STEP;
    fruitY = -BORDER_OFFSET + (initialFruitY + 0.5f) * GRID_STEP;

    currentTime = static_cast<float>(glfwGetTime());
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Delta Time
        prevTime = currentTime;
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - prevTime;

        movementTimer += deltaTime;

        // Update snake position
        if (movementCooldown <= movementTimer)
        {
            movementTimer -= movementCooldown;
            if (!updateSnakePosition())
            {
                std::cout << "Game Over! Final Score: " << score << std::endl;
                break;
            }
        }

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw grid
        drawGrid();

        // Draw border
        drawBorder();

        // Draw fruit
        float fruitColor[] = { 1.0f, 0.0f, 0.0f };
        drawCube(fruitX, fruitY, fruitColor);

        // Draw snake
        float snakeColor[] = { 0.0f, 0.5f, 1.0f };
        drawCube(snake.posX, snake.posY, snakeColor);

        // Render text
        renderText(score);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
