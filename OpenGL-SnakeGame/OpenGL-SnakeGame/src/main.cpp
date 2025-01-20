#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include "rendering.h"
#include "snake.h"
#include "playfield.h"
#include "fruit.h"

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Direction for the snake
glm::vec3 direction(1.0f, 0.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void handleInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        direction = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        direction = glm::vec3(0.0f, -1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        direction = glm::vec3(1.0f, 0.0f, 0.0f);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;


    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snake Game", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Set OpenGL viewport
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize game components
    initSnake();
    initPlayfield();
    initFruit();
    initPlayfield();

    // Compile and link shaders
    unsigned int shaderProgram = createShaderProgram("shaders/vertex_shader.vert", "shaders/fragment_shader.frag");

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Input handling
        handleInput(window);

        // Update game state
        updateSnake(direction);
        checkCollision();

        // Render the scene
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Teal background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Set model, view and projection matrices
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
            glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
            glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
        );
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),         // FOV
            (float)WIDTH / (float)HEIGHT, // Aspect ratio
            0.1f, 100.0f                 // Near and far planes
        );

        // Pass matrices to the shader program
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        int textureLoc = glGetUniformLocation(shaderProgram, "texture1");
        glUniform1i(textureLoc, 0); // Use texture unit 0

        // Render game objects
        renderPlayfield(shaderProgram);
        renderSnake(glm::vec3(0.0f, 0.0f, 0.0f), shaderProgram);
        renderFruit(shaderProgram);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
