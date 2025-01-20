#include "playfield.h"
#include "rendering.h"
#include <glm/glm.hpp>
#include <GLES/gl.h>

// Playfield dimensions
const float PLAYFIELD_SIZE = 10.0f;
unsigned int floorTextureID;
unsigned int borderTextureID;

void initPlayfield() {
    floorTextureID = loadTexture("textures/floor.png");
    borderTextureID = loadTexture("textures/border.png");
}

void renderPlayfield(unsigned int shaderProgram) {
    // Bind the texture for the playfield
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTextureID); // Replace with your loaded texture ID

    // Render the floor
    for (float x = -PLAYFIELD_SIZE / 2; x < PLAYFIELD_SIZE / 2; x++) {
        for (float z = -PLAYFIELD_SIZE / 2; z < PLAYFIELD_SIZE / 2; z++) {
            renderCube(glm::vec3(x, -0.5f, z), shaderProgram);
        }
    }

    // Optionally: Bind a different texture for the borders if needed
    glBindTexture(GL_TEXTURE_2D, borderTextureID); // Replace with your border texture ID

    // Render the borders
    float borderHeight = 1.0f;
    for (float z = -PLAYFIELD_SIZE / 2; z < PLAYFIELD_SIZE / 2; z++) {
        renderCube(glm::vec3(-PLAYFIELD_SIZE / 2, borderHeight, z), shaderProgram);
        renderCube(glm::vec3(PLAYFIELD_SIZE / 2 - 1, borderHeight, z), shaderProgram);
    }
    for (float x = -PLAYFIELD_SIZE / 2; x < PLAYFIELD_SIZE / 2; x++) {
        renderCube(glm::vec3(x, borderHeight, -PLAYFIELD_SIZE / 2), shaderProgram);
        renderCube(glm::vec3(x, borderHeight, PLAYFIELD_SIZE / 2 - 1), shaderProgram);
    }
}
