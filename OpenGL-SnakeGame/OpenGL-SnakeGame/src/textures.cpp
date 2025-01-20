#include "textures.h"
#include <iostream>
#include <GLFW/glfw3.h>

unsigned int loadTexture(const char* filepath) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    // Load the texture image from file
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Bind the texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);

        // Texture wrapping and filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data); // Free image memory after loading
    }
    else {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
