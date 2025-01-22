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

void DirectionalLight::setupDirectionalLight() {
    GLfloat light_direction[] = { -2.0f, -4.0f, -2.0f, 0.0f }; // Direction of the light
    GLfloat light_diffuse[] = { 3.2f, 3.2f, 3.2f, 1.0f };      // Diffuse light intensity
    GLfloat light_specular[] = { 4.0f, 4.0f, 4.0f, 1.0f };     // Specular light intensity
    GLfloat light_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };      // Ambient light intensity

    glEnable(GL_LIGHT0); // Enable the light

    // Set the light properties
    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

void DirectionalLight::setupMaterial() {
    GLfloat ambient[] = { 0.0f, 0.5f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.0f, 0.8f, 0.3f, 1.0f };
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