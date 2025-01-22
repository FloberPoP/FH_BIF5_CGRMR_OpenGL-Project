#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    // Constructor
    Camera(float aspectRatio, float zoomLevel = 1.0f);

    // Getters
    float getZoomLevel() const;
    glm::mat4 getProjectionMatrix() const;

    // Zoom control
    void zoomIn(float targetZoom, float speed, float deltaTime);
    void zoomOut(float targetZoom, float speed, float deltaTime);

    // Update camera logic
    void update(float deltaTime);

    void ToggleZoom();

    bool zoomOffOn;

private:
    float aspectRatio;
    float zoomLevel;
    float targetZoom;
    float zoomSpeed;
    bool isZooming;
    bool zoomOutAfterIn;

    // Helper for smooth interpolation
    void interpolateZoom(float deltaTime);
};


/* Main
Camera camera(static_cast<float>(WIDTH) / HEIGHT);
bool ColorChange = true;

std::string switchConverter(bool b)
{
    if (b)
        return "ON";
    else
        return "OFF";
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

---In processInput:
else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
{
    ColorChange = !ColorChange;

    if(!ColorChange)
        LIGHT.setLightColor(1.0f, 1.0f, 1.0f);

}
else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
{
    camera.ToggleZoom();
}

---In updateSnakePosition:
    camera.zoomIn(0.9f, 8.0f, deltaTime);

---In Render loop befor Rendering objects:
   // Update the camera
   camera.update(deltaTime);

   // Apply the camera's projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glm::mat4 projection = camera.getProjectionMatrix();
   glLoadMatrixf(&projection[0][0]);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

--After Textrender Score:
        TextRenderer::renderTextOnScreen("LightSwitch(C): " + switchConverter(ColorChange) + " | ZoomSwitch(V) " + switchConverter(camera.zoomOffOn), 0.05f, -0.95f, textColor);
*/

