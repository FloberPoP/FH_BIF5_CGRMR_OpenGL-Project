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
#include "../Camera.h"
# define M_PI           3.14159265358979323846

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

GLuint backgroundTexture;
GLuint normalMapTexture;

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

// Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
GLuint VAO, VBO, EBO;

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}

)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// Textures
uniform sampler2D texture1;       // Diffuse texture (brick.jpg)
uniform sampler2D normalMap;      // Normal map (brick_normal.jpg)

// Light properties
uniform vec3 lightPos;            // Light position in world space
uniform vec3 viewPos;             // Camera/view position
uniform vec3 lightColor;          // Light color

void main() {
    // Sample the textures
    vec3 color = texture(texture1, TexCoord).rgb;       // Diffuse color
    vec3 normal = texture(normalMap, TexCoord).rgb;     // Normal from normal map
    normal = normalize(normal * 2.0 - 1.0);             // Transform from [0, 1] to [-1, 1]

    // Lighting calculations
    vec3 lightDir = normalize(lightPos - vec3(0.0, 0.0, 0.0)); // Light direction (simple planar assumption)
    float diff = max(dot(normal, lightDir), 0.0);               // Diffuse intensity
    vec3 diffuse = diff * lightColor;

    // Final color output
    FragColor = vec4(color * diffuse, 1.0f);            // Combine diffuse color with lighting
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

void processInput(GLFWwindow* window);
std::string switchConverter(bool b);

GLuint loadTexture(const char* filePath);
void drawBackground(GLuint texture, GLuint normalMapTexture);
void renderNormalMap();
void drawTexturedQuad(float x, float y, float w, float h, float dirX, float dirY, const float* color, GLuint texture);

GLuint snakeTexture;
GLuint grassTexture;
GLuint FruitTextures[10];

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

    initShaders();
    initGL();

    // Load Textures
    backgroundTexture = loadTexture("assets/brick.jpg");
    normalMapTexture = loadTexture("assets/brick_normal.jpg");
    snakeTexture = loadTexture("assets/snakeBody.png");
    grassTexture = loadTexture("assets/grass.jpg");

    FruitTextures[FruitE::Cherry] = loadTexture("assets/cherry.png");
    FruitTextures[FruitE::Banana] = loadTexture("assets/banana.png");
    FruitTextures[FruitE::Meat] = loadTexture("assets/meat.png");
    FruitTextures[FruitE::Eggplant] = loadTexture("assets/eggplant.png");
    FruitTextures[FruitE::Beer] = loadTexture("assets/beer.png");
    FruitTextures[FruitE::Orange] = loadTexture("assets/orange.png");
    FruitTextures[FruitE::Pineapple] = loadTexture("assets/pineapple.png");
    FruitTextures[FruitE::Peach] = loadTexture("assets/peach.png");
    FruitTextures[FruitE::Brocoli] = loadTexture("assets/brocoli.png");
    FruitTextures[FruitE::Pear] = loadTexture("assets/pear.png");
        
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
void initShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Initialize OpenGL settings
void initGL()
{
    // Define vertices (positions and texture coordinates)
    float vertices[] = {
        // Positions      // Texture Coords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // Top-right
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // Top-left
    };

    // Define indices for two triangles
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Generate and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Bind and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture Coords
    glEnableVertexAttribArray(1);

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
    // Render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw Background with NormalMap
    renderNormalMap();
    float color[] = { 1.0f, 1.0f, 1.0f };
    drawTexturedQuad(0, 0, Grid::BORDER_OFFSET*2, Grid::BORDER_OFFSET*2, 0, 0, color, grassTexture);

    // Draw grid
    drawGrid();

    // Draw border
    drawBorder();

    // Draw fruits
    for (Fruit fruit : fruits)
    {
        drawTexturedQuad(fruit.GetPos().x, fruit.GetPos().y, SNAKE_SIZE, SNAKE_SIZE, 0, 1, color, FruitTextures[fruit.fruitType]);
    }

    // Draw snake
    drawSnake();

    // Render text
    float textColor[] = { 1.0f, 1.0f, 1.0f };
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
            //drawQuad(sp->pos.x, sp->pos.y, SNAKE_SIZE, SNAKE_SIZE, snakeBodyColor);

            
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glm::vec2 dir = glm::normalize(sp->prev->pos - sp->pos);
            drawTexturedQuad(sp->pos.x, sp->pos.y, SNAKE_SIZE, SNAKE_SIZE, dir.x, dir.y, snakeBodyColor, snakeTexture);
            
        }
    }
    drawSnakeHead(snakeHeadColor);
}

std::string switchConverter(bool b) {
    if (b)
        return "ON";
    else
        return "OFF";
}

GLuint loadTexture(const char* filePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 4);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

void drawTexturedQuad(float x, float y, float w, float h, float dirX, float dirY, const float* color, GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor3d(color[0], color[1], color[2]);

    if (dirY != 0)
    {
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(x - w / 2, y - h / 2); // bottom left
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(x + w / 2, y - h / 2); // bottom right
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(x + w / 2, y + h / 2); // top right
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(x - w / 2, y + h / 2); // top left
    }
    else
    {
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(x - w / 2, y - h / 2); // bottom left
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(x + w / 2, y - h / 2); // bottom right
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(x + w / 2, y + h / 2); // top right
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(x - w / 2, y + h / 2); // top left
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBackground(GLuint texture, GLuint normalMapTexture) {
    // Use the shader program
    glUseProgram(shaderProgram);

    // Set textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);

    // Set uniforms for lighting
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 0.0f, 3.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);

    // Bind VAO and draw elements
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void renderNormalMap() {
    // Use the shader program
    glUseProgram(shaderProgram);

    // Set uniform values
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 0.0f, 3.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);

    // Draw the background with normal mapping
    drawBackground(backgroundTexture, normalMapTexture);
    glUseProgram(0);
}