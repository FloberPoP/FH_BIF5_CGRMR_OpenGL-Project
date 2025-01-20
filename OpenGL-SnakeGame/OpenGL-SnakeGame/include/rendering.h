#ifndef RENDERING_H
#define RENDERING_H

#include <glm/glm.hpp>

// Function to render a cube at a given position with a specific shader program
void renderCube(glm::vec3 position, unsigned int shaderProgram);
void setupCube();

#endif // RENDERING_H
