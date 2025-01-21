#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "glm/glm.hpp"
#include "../Snake.h"
#include "../Grid.h"

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

Grid GRID = Grid();

const float SNAKE_SIZE = GRID.GRID_STEP * 0.9f; // Snake size relative to grid field

Snake snake = Snake(GRID.GRID_STEP);

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
Fruit fruit;

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
    for (int i = 0; i <= GRID.GRID_SIZE; ++i)
    {
        float coord = -GRID.BORDER_OFFSET + i * GRID.GRID_STEP;
        // Vertical lines
        glVertex2f(coord, -GRID.BORDER_OFFSET);
        glVertex2f(coord, GRID.BORDER_OFFSET);
        // Horizontal lines
        glVertex2f(-GRID.BORDER_OFFSET, coord);
        glVertex2f(GRID.BORDER_OFFSET, coord);
    }
    glEnd();
}

// Border rendering
void drawBorder()
{
    glColor3f(1.0f, 0.0f, 0.0f); // Border color
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-GRID.BORDER_OFFSET, -GRID.BORDER_OFFSET);
    glVertex2f(GRID.BORDER_OFFSET, -GRID.BORDER_OFFSET);
    glVertex2f(GRID.BORDER_OFFSET, GRID.BORDER_OFFSET);
    glVertex2f(-GRID.BORDER_OFFSET, GRID.BORDER_OFFSET);
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
        snake.dir.x = 0.0f;
        snake.dir.y = 1.0f;
        lastDirInput = GLFW_KEY_W;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && lastDirInput != GLFW_KEY_W)
    {
        snake.dir.x = 0.0f;
        snake.dir.y = -1.0f;
        lastDirInput = GLFW_KEY_S;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && lastDirInput != GLFW_KEY_D)
    {
        snake.dir.x = -1.0f;
        snake.dir.y = 0.0f;
        lastDirInput = GLFW_KEY_A;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && lastDirInput != GLFW_KEY_A)
    {
        snake.dir.x = 1.0f;
        snake.dir.y = 0.0f;
        lastDirInput = GLFW_KEY_D;
    }
}

// Update snake position and ensure it stays within bounds
bool updateSnakePosition()
{
    // Update position
    snake.ApplyMovement();
    
    if (snake.CollidesWithBorder())
    {
        return false;
    }

    if (snake.CollidesWithSelf())
    {
        return false;
    }

    if (snake.CollidesWithFruit(fruit))
    {
        score++;
        // Move the fruit to a new random position within the grid
        int gridX = rand() % Grid::GRID_SIZE;
        int gridY = rand() % Grid::GRID_SIZE;
        fruitX = -Grid::BORDER_OFFSET + (gridX + 0.5f) * Grid::GRID_STEP;
        fruitY = -Grid::BORDER_OFFSET + (gridY + 0.5f) * Grid::GRID_STEP;
    }

    return true;
}

int main()
{
    Grid GRID = Grid();
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
    int initialFruitX = rand() % GRID.GRID_SIZE;
    int initialFruitY = rand() % GRID.GRID_SIZE;
    fruitX = -GRID.BORDER_OFFSET + (initialFruitX + 0.5f) * GRID.GRID_STEP;
    fruitY = -GRID.BORDER_OFFSET + (initialFruitY + 0.5f) * GRID.GRID_STEP;

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
        drawCube(snake.GetPos().x, snake.GetPos().y, snakeColor);

        // Render text
        renderText(score);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
