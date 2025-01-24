#include "DirectionalLight.h"
#include <iostream>

DirectionalLight::DirectionalLight() : active(true) {}

void DirectionalLight::toggle() {
    active = !active;
    if (active)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);
}

void DirectionalLight::setLightColor(float red, float green, float blue) {
    GLfloat light_direction[] = { -2.0f, -4.0f, -2.0f, 0.0f }; // Direction of the light
    GLfloat light_diffuse[] = { red * 2.0f, green * 2.0f, blue * 2.0f, 1.0f };  // Scale the color for brightness
    GLfloat light_specular[] = { red * 2.0f, green * 2.0f, blue * 2.0f, 1.0f }; // Scale for highlights
    GLfloat light_ambient[] = { red * 0.8f, green * 0.8f, blue * 0.8f, 1.0f };  // Dim ambient light

    glEnable(GL_LIGHT0); // Enable the light

    // Set the light properties
    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

void DirectionalLight::setupMaterial() {
    GLfloat ambient[] = { 0.3f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuse[] = { 0.7f, 0.0f, 0.0f, 1.0f };
    GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat shininess = 50.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

bool DirectionalLight::isActive() const {
    return active;
}


