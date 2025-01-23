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
#include <vector>
# define M_PI           3.14159265358979323846

#include "stb_image.h"
#include "../snake.h"
#include "../Grid.h"
#include "../TextRenderer.h"
#include "../DirectionalLight.h"
#include "../Camera.h"

#include "../VertexBuffer.h"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../Renderer.h"
#include "../Texture.h"

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

Camera camera(static_cast<float>(WIDTH) / HEIGHT);
bool ColorChange = true;

// Shader program
GLuint shaderProgram;

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoords);
}
)";

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

void drawTexture(float x, float y, float w, float h);

void drawCherry(float spawnX, float spawnY);
void drawBanana(float spawnX, float spawnY);

void drawSnake();
void drawSnakeHead(const float* color);
void drawSnakeTail(const float* color);

void processInput(GLFWwindow* window);
std::string switchConverter(bool b);

Renderer renderer;

Shader* RoboShader;
Texture* RoboTexture;


GLFWwindow* InitWindow()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Snake", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return nullptr;

    }
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return window;
}

int main(int argc, char** argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);

    GLFWwindow* window = InitWindow();
    if (!window)
        return -1;

    
    Grid GRID = Grid();
    snake = new Snake();
    fruits.push_back(Fruit());
    currentTime = static_cast<float>(glfwGetTime());

    
    // Setup the directional light
    LIGHT.setupDirectionalLight();
    glEnable(GL_LIGHTING);   // Enable lighting
    glEnable(GL_COLOR_MATERIAL); // Allows glColor to interact with light
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    RoboShader = new Shader("shaders/Basic.shader");
    RoboTexture = new Texture("assets/robo.png");
    
    do
    {
        updateGame(window);
        
        renderGame();

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));

    // Clean up
    delete snake;
    delete RoboShader;
    delete RoboTexture;

    glfwTerminate();
    return 0;
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

    // Update the camera
    camera.update(deltaTime);

    // Apply the camera's projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 projection = camera.getProjectionMatrix();
    glLoadMatrixf(&projection[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Render the game
void renderGame()
{
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Base background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw grid
    drawGrid();

    // Draw border
    drawBorder();

    float textColor[] = { 1.0f, 1.0f, 1.0f };
    // Draw fruits
    for (Fruit fruit : fruits)
    {
        //drawTrinagle(fruit.GetPos().x, fruit.GetPos().y, SNAKE_SIZE, SNAKE_SIZE, 0,1, textColor);
        drawTexture(fruit.GetPos().x, fruit.GetPos().y, SNAKE_SIZE, SNAKE_SIZE);

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
    TextRenderer::renderTextOnScreen("Score: " + std::to_string(score), -0.9f, -0.95f, textColor);
    TextRenderer::renderTextOnScreen("LightSwitch(C): " + switchConverter(ColorChange) + " | ZoomSwitch(V) " + switchConverter(camera.zoomOffOn), 0.05f, -0.95f, textColor);
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
    if (ColorChange)
    {
        float red = (sin(elapsedTime) + 1.0f) / 2.0f;   // Oscillates between 0 and 1
        float green = (sin(elapsedTime + 2.0f) + 1.0f) / 2.0f; // Phase shift for variety
        float blue = (sin(elapsedTime + 4.0f) + 1.0f) / 2.0f;  // Another phase shift

        LIGHT.setLightColor(red, green, blue);
    }
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
    ColorChange = true;
    camera.zoomOffOn = true;
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
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        ColorChange = !ColorChange;

        if (!ColorChange)
            LIGHT.setLightColor(1.0f, 1.0f, 1.0f);

    }
    else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        camera.ToggleZoom();
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
            camera.zoomIn(0.9f, 8.0f, deltaTime);
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

std::string switchConverter(bool b)
{
    if (b)
        return "ON";
    else
        return "OFF";
}

void drawTexture(float x, float y, float w, float h)
{
    RoboShader->Bind();
    RoboTexture->Bind();
    RoboShader->SetUniform1i("u_Texture", 0);

    float positions[] = {
         x - w / 2, y - h / 2, 0.0f, 0.0f, // 0
         x + w / 2, y - h / 2, 1.0f, 0.0f, // 1
         x + w / 2, y + h / 2, 1.0f, 1.0f, // 2
         x - w / 2, y + h / 2, 0.0f, 1.0f  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    IndexBuffer ib(indices, 6);

    VertexBufferLayout layout;
    layout.AddFloat(2);
    layout.AddFloat(2);

    va.AddBuffer(vb, layout);

    renderer.Clear();
    renderer.Draw(va, ib, *RoboShader);
}