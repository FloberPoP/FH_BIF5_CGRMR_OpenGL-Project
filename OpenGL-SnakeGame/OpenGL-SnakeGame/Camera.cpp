#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// Constructor
Camera::Camera(float aspectRatio, float zoomLevel)
    : aspectRatio(aspectRatio), zoomLevel(zoomLevel), targetZoom(zoomLevel),
    zoomSpeed(0.0f), isZooming(false), zoomOutAfterIn(false), zoomOffOn(true) {}

// Get the current zoom level
float Camera::getZoomLevel() const {
    return zoomLevel;
}

// Generate the projection matrix
glm::mat4 Camera::getProjectionMatrix() const {
    return glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel,
        -1.0f * zoomLevel, 1.0f * zoomLevel);
}

// Trigger a zoom-in effect
void Camera::zoomIn(float targetZoom, float speed, float deltaTime) {
    if (zoomOffOn)
    {
        this->targetZoom = targetZoom;
        this->zoomSpeed = speed;
        this->isZooming = true;
        this->zoomOutAfterIn = true;
    }
}

// Trigger a zoom-out effect
void Camera::zoomOut(float targetZoom, float speed, float deltaTime) {
    this->targetZoom = targetZoom;
    this->zoomSpeed = speed;
    this->isZooming = true;
}

// Update the camera zoom
void Camera::update(float deltaTime) {
    if (isZooming) {
        interpolateZoom(deltaTime);
    }
}

// Interpolate zoom smoothly
void Camera::interpolateZoom(float deltaTime) {
    zoomLevel += (targetZoom - zoomLevel) * zoomSpeed * deltaTime;

    // Check if zoom is near the target
    if (std::fabs(zoomLevel - targetZoom) < 0.01f) {
        zoomLevel = targetZoom;

        // Transition to zoom-out if required
        if (zoomOutAfterIn && targetZoom != 1.0f) { // Prevent endless zooming
            targetZoom = 1.0f;      // Set zoom-out target
            zoomOutAfterIn = false; // Clear the flag
        }
        else {
            isZooming = false; // End zoom effect
        }
    }
}

void Camera::ToggleZoom()
{
    zoomOffOn = !zoomOffOn;
}