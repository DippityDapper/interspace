#include "game/Camera.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/Input.hpp"

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
        float mSpeed = moveSpeed * 32 / zoom;

        if (Input::IsKeyDown(SDLK_LSHIFT))
            mSpeed *= 2;

        mSpeed *= delta;

        if (Input::IsKeyDown(SDLK_W))
            targetPosition.y -= mSpeed;
        if (Input::IsKeyDown(SDLK_S))
            targetPosition.y += mSpeed;
        if (Input::IsKeyDown(SDLK_A))
            targetPosition.x -= mSpeed;
        if (Input::IsKeyDown(SDLK_D))
            targetPosition.x += mSpeed;

        float tPos = std::min(delta * panSpeed, 1.0f);
        float tZoom = std::min(delta * zoomSpeed, 1.0f);

        if (targetPosition != position)
        {
            position = position + (targetPosition - position) * tPos;
            if (position.DistanceTo(targetPosition) < 0.1f)
                position = targetPosition;

            if (limitBounds)
                targetPosition = ClampToBounds(targetPosition, targetZoom);
        }
        if (targetZoom != zoom)
        {
            zoom = zoom + (targetZoom - zoom) * tZoom;
            if (std::abs(targetZoom - zoom) < 0.01f)
                zoom = targetZoom;

            if (limitBounds)
                targetZoom = ClampToBounds(targetZoom);
        }
    }

    void Game::Camera::HandleEvents(SDL_Event &event)
    {
        if (event.type == SDL_EVENT_MOUSE_MOTION && Input::IsMouseButtonDown(SDL_BUTTON_MIDDLE))
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

            if (limitBounds)
            {
                targetZoom = ClampToBounds(targetZoom);
                targetPosition = ClampToBounds(targetPosition, targetZoom);
            }
        }
    }
}