#include "Client/Camera.hpp"

#include "SDL3/SDL.h"

// TODO : Move camera smoothing to the engine camera, make it general use

namespace Game
{
    Camera::Camera() : Engine::Camera()
    {
        targetPosition = position;
        targetZoom = zoom;
    }

    Camera::Camera(float x, float y, float zoom) : Engine::Camera(x, y, zoom)
    {
        targetPosition = position;
        targetZoom = zoom;
    }

    void Camera::Update(float delta)
    {
        float tPos = std::min(delta * panSpeed, 1.0f);
        float tZoom = std::min(delta * zoomSpeed, 1.0f);

        if (targetPosition != position)
        {
            position = position + (targetPosition - position) * tPos;
            if (position.DistanceTo(targetPosition) < 0.1f)
                position = targetPosition;
        }
        if (targetZoom != zoom)
        {
            zoom = zoom + (targetZoom - zoom) * tZoom;
            if (std::abs(targetZoom - zoom) < 0.01f)
                zoom = targetZoom;
        }
    }

    void Game::Camera::HandleEvents(SDL_Event &event)
    {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                panning = true;
            }
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                panning = false;
            }
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION && panning)
        {
            targetPosition.x -= event.motion.xrel / zoom;
            targetPosition.y -= event.motion.yrel / zoom;
            targetPosition = ClampToBounds(targetPosition);
        }
        if (event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            if (event.wheel.y > 0)
                targetZoom *= 1.1f;
            else if (event.wheel.y < 0)
                targetZoom *= 0.9f;

            if (targetZoom < minZoom)
                targetZoom = minZoom;
            if (targetZoom > maxZoom)
                targetZoom = maxZoom;

            targetZoom = ClampToBounds(targetZoom);
            targetPosition = ClampToBounds(targetPosition, targetZoom);
        }
    }
}