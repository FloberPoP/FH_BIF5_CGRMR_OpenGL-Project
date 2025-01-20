#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>

// Utility functions for shaders
std::string readFile(const char* filePath);
unsigned int compileShader(const char* source, GLenum shaderType);
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);

#endif // SHADER_H
