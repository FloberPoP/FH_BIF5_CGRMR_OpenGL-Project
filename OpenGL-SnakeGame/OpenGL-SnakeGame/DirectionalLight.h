#pragma once
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

class DirectionalLight
{
public:
    DirectionalLight();

    void toggle();
    void setupDirectionalLight();
    void setupMaterial();
    bool isActive() const;
    void setLightColor(float red, float green, float blue);

private:
    bool active;
};