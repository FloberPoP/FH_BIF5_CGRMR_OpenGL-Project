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

