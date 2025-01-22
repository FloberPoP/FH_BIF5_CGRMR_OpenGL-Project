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
#include <GL/freeglut.h>
#include "../Snake.h"
#include "../Grid.h"
# define M_PI           3.14159265358979323846

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

Grid GRID = Grid();

const float SNAKE_SIZE = GRID.GRID_STEP * 0.9f; // Snake size relative to grid field

Snake snake = Snake();

int lastDirInput = 0;
int currentDirInput = 0;
float movementCooldown = 0.2f; // in sec
float movementTimer = 0.0f;
float prevTime;
float currentTime;
float deltaTime;

// Counter for collisions
int score = 0;

// Fruit position
// float fruitX = -0.85f;
// float fruitY = -0.85f;
Fruit fruit = Fruit();

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

// Function to draw text at a specific position
void renderTextOnScreen(const std::string& text, float x, float y, const float* color)
{
    glColor3fv(color); // Set the text color
    glRasterPos2f(x, y); // Set the position for text rendering

    // Render each character
    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
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

// Process input to change direction
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && lastDirInput != GLFW_KEY_S)
    {
        snake.dir.x = 0.0f;
        snake.dir.y = 1.0f;
        currentDirInput = GLFW_KEY_W;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && lastDirInput != GLFW_KEY_W)
    {
        snake.dir.x = 0.0f;
        snake.dir.y = -1.0f;
        currentDirInput = GLFW_KEY_S;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && lastDirInput != GLFW_KEY_D)
    {
        snake.dir.x = -1.0f;
        snake.dir.y = 0.0f;
        currentDirInput = GLFW_KEY_A;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && lastDirInput != GLFW_KEY_A)
    {
        snake.dir.x = 1.0f;
        snake.dir.y = 0.0f;
        currentDirInput = GLFW_KEY_D;
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
        snake.Grow();
        fruit = Fruit();
    }

    return true;
}

void drawQuad(float x, float y, float w, float h, const float* color)
{
    glColor3fv(color);
    glBegin(GL_QUADS);
    glVertex2f(x - w / 2, y - h / 2);
    glVertex2f(x + w / 2, y - h / 2);
    glVertex2f(x + w / 2, y + h / 2);
    glVertex2f(x - w / 2, y + h / 2);
    glEnd();
}

void drawTrinagle(float x, float y, float w, float h, float dirX, float dirY, const float* color)
{
    glColor3fv(color);

    float halfWidth = w / 2.0f;
    float halfHeight = h / 2.0f;

    glBegin(GL_TRIANGLES);

    // Top vertex
    glVertex2f(x + halfWidth * dirX * -1, y + halfHeight * dirY * -1);

    // Bottom-left vertex
    glVertex2f(
        (dirX != 0) ? x + halfWidth * dirX : x - halfWidth,
        (dirY != 0) ? y + halfHeight * dirY : y - halfHeight
    );

    // Bottom-right vertex
    glVertex2f(
        (dirX != 0) ? x + halfWidth * dirX : x + halfWidth,
        (dirY != 0) ? y + halfHeight * dirY : y + halfHeight
    );

    glEnd();
}

void drawCircle(float x, float y, float radius, const float* color) {
    glColor3fv(color);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; ++i) {
        float angle = i * 2.0f * M_PI / 100.0f;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// Function to draw a line (for the stem)
void drawLine(float x1, float y1, float x2, float y2, const float* color) {
    glColor3fv(color);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawCherry(float spawnX, float spawnY) 
{
    // Draw the cherries
    float cherryColor[] = { 1.0f, 0.0f, 0.0f };
    drawCircle(spawnX - 0.01f, spawnY - 0.01f, 0.01f, cherryColor); // Left cherry (red)
    drawCircle(spawnX + 0.01f, spawnY - 0.01f, 0.01f, cherryColor);  // Right cherry (red)

    // Draw the stems
    float stemColor[] = { 0.0f, 0.5f, 0.0f };
    drawLine(spawnX - 0.01f, spawnY - 0.015f, spawnX + 0.0f, spawnY + 0.02f, stemColor); // Left stem (green)
    drawLine(spawnX + 0.01f, spawnY - 0.015f, spawnX + 0.0f, spawnY + 0.02f, stemColor);  // Right stem (green)

    // Draw the connecting part of the stems
    drawLine(spawnX + 0.005f, spawnY + 0.02f, spawnX - 0.005f, spawnY + 0.02f, stemColor); // Connection (green)
}
void drawBanana(float spawnX, float spawnY)
{
    // Draw the banana
    float bananaColor1[] = { 1.0f , 0.984f, 0.0f };
    float bananaColor2[] = { 0.769f, 0.741f, 0.027f };
    float bananaColor3[] = { 0.2f, 0.302f, 0.302f };

    drawCircle(spawnX - 0.00f, spawnY - 0.00f, 0.02f, bananaColor1);
    drawCircle(spawnX + 0.005f, spawnY + 0.005f, 0.015f, bananaColor2);
    drawCircle(spawnX + 0.01f, spawnY + 0.01f, 0.015f, bananaColor3);

    // Draw the end
    float endColor[] = { 0.2f, 0.11f, 0.067f };
    drawLine(spawnX - 0.01f, spawnY + 0.02f, spawnX + 0.001f, spawnY + 0.02f, endColor); // Left stem (green)
    drawLine(spawnX + 0.02f, spawnY - 0.00f, spawnX + 0.02f, spawnY - 0.01f, endColor);  // Right stem (green)
}

void drawSnakeHead(const float* color)
{
    drawCircle(snake.GetHead().pos.x, snake.GetHead().pos.y, SNAKE_SIZE / 2, color);

    float quadWidth = (snake.dir.x != 0) ? SNAKE_SIZE / 2 : SNAKE_SIZE;
    float quadHeight = (snake.dir.y != 0) ? SNAKE_SIZE / 2 : SNAKE_SIZE;
    float quadPosX = snake.GetHead().pos.x - (snake.dir.x * SNAKE_SIZE / 4);
    float quadPosY = snake.GetHead().pos.y - (snake.dir.y * SNAKE_SIZE / 4);

    drawQuad(
        quadPosX,
        quadPosY,
        quadWidth, quadHeight, color);
}

void drawSnakeTail(const float* color)
{
    drawTrinagle(snake.GetTail()->pos.x, snake.GetTail()->pos.y, SNAKE_SIZE, SNAKE_SIZE, snake.dir.x, snake.dir.y, color);
}

void drawSnake()
{
    float snakeBodyColor[] = { 0.0f, 1.0f, 0.5f };
    float snakeHeadColor[] = { 0.0f, 1.0f, 0.2f };

    if (score > 0)
    {
        drawSnakeTail(snakeBodyColor);
        for (SnakePart* sp = snake.GetTail()->prev; sp->prev != nullptr; sp = sp->prev)
        {
            drawQuad(sp->pos.x, sp->pos.y, SNAKE_SIZE, SNAKE_SIZE, snakeBodyColor);
        }
    }
    drawSnakeHead(snakeHeadColor);
}

int main(int argc, char** argv)
{
    Grid GRID = Grid();

    // Initialize GLUT
    glutInit(&argc, argv);

    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

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
    /*int initialFruitX = rand() % GRID.GRID_SIZE;
    int initialFruitY = rand() % GRID.GRID_SIZE;
    fruitX = -GRID.BORDER_OFFSET + (initialFruitX + 0.5f) * GRID.GRID_STEP;
    fruitY = -GRID.BORDER_OFFSET + (initialFruitY + 0.5f) * GRID.GRID_STEP;*/

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
            lastDirInput = currentDirInput;
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
        //drawCube(fruit.GetPos().x, fruit.GetPos().y, fruitColor);
        //drawCherry(fruit.GetPos().x, fruit.GetPos().y);
        drawBanana(fruit.GetPos().x, fruit.GetPos().y);

        // Draw snake
        drawSnake();

        // Render text
        float textColor[] = { 1.0f, 1.0f, 1.0f };
        renderTextOnScreen("Score: " + std::to_string(score), -0.9f, -0.95f, textColor);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
