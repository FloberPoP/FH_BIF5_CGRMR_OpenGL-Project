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
#include "../snake.h"
#include "../Grid.h"
#include "../TextRenderer.h"
#include <vector>
#include "../DirectionalLight.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
# define M_PI           3.14159265358979323846

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

Grid GRID = Grid();

DirectionalLight LIGHT = DirectionalLight();

const float SNAKE_SIZE = GRID.GRID_STEP * 0.9f; // Snake size relative to grid field

Snake* snake;
std::vector<Fruit> fruits;

int lastDirInput = 0;
int currentDirInput = 0;
float movementCooldown = 0.2f; // in sec
float movementTimer = 0.0f;
float prevTime;
float currentTime;
float deltaTime;

// Counter for collisions
int score = 0;
bool OnHit = false;

// Shader program
GLuint shaderProgram;

// Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
GLuint VAO, VBO, EBO;

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 offset;

void main()
{
    gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

void initShaders();
void initGL();
void processInput(GLFWwindow* window);
bool updateSnakePosition();
void resetGame();
void renderRestartPrompt();
void updateGame(GLFWwindow* window);
void renderGame();
void lostGame(GLFWwindow* window);

void updateLightColorOverTime(float elapsedTime);
void renderRestartPrompt();

void drawGrid();
void drawBorder();
void drawQuad(float x, float y, float w, float h, const float* color);
void drawTrinagle(float x, float y, float w, float h, float dirX, float dirY, const float* color);
void drawCircle(float x, float y, float radius, const float* color);
void drawLine(float x1, float y1, float x2, float y2, const float* color);

void drawCherry(float spawnX, float spawnY);
void drawBanana(float spawnX, float spawnY);

void drawSnake();
void drawSnakeHead(const float* color);
void drawSnakeTail(const float* color);

int main(int argc, char** argv)
{
    Grid GRID = Grid();
    snake = new Snake();
    fruits.push_back(Fruit());
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

    currentTime = static_cast<float>(glfwGetTime());

    // Setup the directional light
    LIGHT.setupDirectionalLight();
    glEnable(GL_LIGHTING);   // Enable lighting
    glEnable(GL_COLOR_MATERIAL); // Allows glColor to interact with light
    glEnable(GL_FRAMEBUFFER_SRGB);

    //initGL();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        updateGame(window);

        renderGame();

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    delete snake;
    glfwTerminate();
    return 0;
}

// Initialize shaders and OpenGL objects
void initShaders()
{
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Link shaders into a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Initialize OpenGL settings
void initGL()
{
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Initialize shaders
    initShaders();

    // Setup VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Bind and set up VBO and EBO here...
    // TODO

    glBindVertexArray(0);
}

// Update game state
void updateGame(GLFWwindow* window)
{
    float elapsedTime = static_cast<float>(glfwGetTime());

    // Update light color
    updateLightColorOverTime(elapsedTime);

    // Input
    processInput(window);

    // Delta Time
    prevTime = currentTime;
    currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - prevTime;

    movementTimer += deltaTime;

    // Update snake position
    if (movementCooldown <= movementTimer) {
        movementTimer -= movementCooldown;
        if (!updateSnakePosition()) {
            lostGame(window);
        }
    }
    lastDirInput = currentDirInput;
}

// Render the game
void renderGame()
{
    // Render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw grid
    drawGrid();

    // Draw border
    drawBorder();

    // Draw fruits
    for (Fruit fruit : fruits)
    {
        switch (fruit.fruitType)
        {
        case 0: // Cherry
            drawCherry(fruit.GetPos().x, fruit.GetPos().y);
            break;
        case 1: // Banana
            drawBanana(fruit.GetPos().x, fruit.GetPos().y);
            break;
        default:
            drawBanana(fruit.GetPos().x, fruit.GetPos().y);
            break;
        }
    }

    // Draw snake
    drawSnake();

    // Render text
    float textColor[] = { 1.0f, 1.0f, 1.0f };
    TextRenderer::renderTextOnScreen("Score: " + std::to_string(score), -0.9f, -0.95f, textColor);
}

// Lost Game
void lostGame(GLFWwindow* window)
{
    std::cout << "Game Over! Final Score: " << score << std::endl;

    // Write score to file
    TextRenderer::writeScoreToFile(score);

    // Read and display the scoreboard
    std::vector<int> scores = TextRenderer::readScoresFromFile();

    // End-game rendering loop
    bool restart = false;
    while (!restart) {
        if (!LIGHT.isActive()) {
            LIGHT.toggle(); // Ensure the light is active
        }

        // Reset the light to a default state (perfect white light)
        LIGHT.setLightColor(1.0f, 1.0f, 1.0f); // Reset to white light

        // Render the scoreboard
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Background color
        glClear(GL_COLOR_BUFFER_BIT);
        TextRenderer::renderScoreboard(scores, score);
        renderRestartPrompt();
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Check for restart or exit input
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            resetGame();  // Reset game state
            restart = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);  // Close the game
            restart = true;
        }
    }
}

void updateLightColorOverTime(float elapsedTime) {
    float red = (sin(elapsedTime) + 1.0f) / 2.0f;   // Oscillates between 0 and 1
    float green = (sin(elapsedTime + 2.0f) + 1.0f) / 2.0f; // Phase shift for variety
    float blue = (sin(elapsedTime + 4.0f) + 1.0f) / 2.0f;  // Another phase shift

    LIGHT.setLightColor(red, green, blue);
}

// Render the restart button text
void renderRestartPrompt() {
    float textColor[] = { 1.0f, 1.0f, 0.0f }; // Yellow color for visibility
    TextRenderer::renderTextOnScreen("Press 'R' to Restart or 'ESC' to Exit", -0.5f, -0.8f, textColor);
}

// Reset the game state for a restart
void resetGame() {
    delete snake;
    snake = new Snake();        // Reset the snake
    fruits.clear();             // Reset the fruits
    score = 0;                  // Reset the score
    lastDirInput = 0;           // Reset direction input
    currentDirInput = 0;
    movementTimer = 0.0f;       // Reset movement timer
}

// Grid rendering
void drawGrid()
{
    LIGHT.setupMaterial();
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
    LIGHT.setupMaterial();
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
        snake->dir.x = 0.0f;
        snake->dir.y = 1.0f;
        currentDirInput = GLFW_KEY_W;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && lastDirInput != GLFW_KEY_W)
    {
        snake->dir.x = 0.0f;
        snake->dir.y = -1.0f;
        currentDirInput = GLFW_KEY_S;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && lastDirInput != GLFW_KEY_D)
    {
        snake->dir.x = -1.0f;
        snake->dir.y = 0.0f;
        currentDirInput = GLFW_KEY_A;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && lastDirInput != GLFW_KEY_A)
    {
        snake->dir.x = 1.0f;
        snake->dir.y = 0.0f;
        currentDirInput = GLFW_KEY_D;
    }
}

// Update snake position and ensure it stays within bounds
bool updateSnakePosition()
{
    // Update position
    snake->ApplyMovement();
    
    if (snake->CollidesWithBorder())
    {
        return false;
    }

    if (snake->CollidesWithSelf())
    {
        return false;
    }

    for (std::vector<Fruit>::iterator it = fruits.begin(); it != fruits.end(); ++it) {
        if (snake->CollidesWithFruit(*it))
        {
            ++score;
            snake->Grow();
            *it = Fruit();
            OnHit = true;
        }
    }
    if (OnHit && score % 10 == 0)
    {
        fruits.push_back(Fruit());
        OnHit = false;
    }
    return true;
}

void drawQuad(float x, float y, float w, float h, const float* color)
{
    LIGHT.setupMaterial();
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
    LIGHT.setupMaterial();
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
    LIGHT.setupMaterial();
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
    LIGHT.setupMaterial();
    glColor3fv(color);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawCherry(float spawnX, float spawnY) 
{
    LIGHT.setupMaterial();
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
    LIGHT.setupMaterial();
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
    drawCircle(snake->GetHead().pos.x, snake->GetHead().pos.y, SNAKE_SIZE / 2, color);

    float quadWidth = (snake->dir.x != 0) ? SNAKE_SIZE / 2 : SNAKE_SIZE;
    float quadHeight = (snake->dir.y != 0) ? SNAKE_SIZE / 2 : SNAKE_SIZE;
    float quadPosX = snake->GetHead().pos.x - (snake->dir.x * SNAKE_SIZE / 4);
    float quadPosY = snake->GetHead().pos.y - (snake->dir.y * SNAKE_SIZE / 4);

    drawQuad(
        quadPosX,
        quadPosY,
        quadWidth, quadHeight, color);
}

void drawSnakeTail(const float* color)
{
    glm::vec2 dir = snake->GetTailDir();
    drawTrinagle(snake->GetTail()->pos.x, snake->GetTail()->pos.y, SNAKE_SIZE, SNAKE_SIZE, dir.x, dir.y, color);
}

void drawSnake()
{
    LIGHT.setupMaterial();
    float snakeBodyColor[] = { 0.0f, 1.0f, 0.5f };
    float snakeHeadColor[] = { 0.0f, 1.0f, 0.2f };

    if (score > 0)
    {
        drawSnakeTail(snakeBodyColor);
        for (SnakePart* sp = snake->GetTail()->prev; sp->prev != nullptr; sp = sp->prev)
        {
            drawQuad(sp->pos.x, sp->pos.y, SNAKE_SIZE, SNAKE_SIZE, snakeBodyColor);
        }
    }
    drawSnakeHead(snakeHeadColor);
}
