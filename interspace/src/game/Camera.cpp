#include "interspace/game/Camera.hpp"

#include <algorithm>

#include "SDL3/SDL.h"

#include "igneous/input/Input.hpp"
#include "igneous/rendering/Window.hpp"
#include "interspace/game/Game.hpp"
#include "interspace/world/WorldInterface.hpp"

namespace Interspace
{
    Camera::Camera()
    {
        targetPosition = position;
        targetZoom = zoom;
        oldViewport = Engine::Window::viewport;
    }

    Camera::Camera(float x, float y, float zoom) : Engine::Camera(x, y, zoom)
    {
        targetPosition = position;
        targetZoom = zoom;
        oldViewport = Engine::Window::viewport;
    }

    void Camera::Update(float delta)
    {
        float tPos = std::min(delta * panSpeed, 1.0f);
        float tZoom = std::min(delta * zoomSpeed, 1.0f);

        Engine::Vec2<float> velocityNorm = velocity.Normalized();
        targetPosition.y += velocityNorm.y * (moveSpeed * Client::World::worldData->TILE_SIZE / zoom) * moveSpeedMultiplier * delta;
        targetPosition.x += velocityNorm.x * (moveSpeed * Client::World::worldData->TILE_SIZE / zoom) * moveSpeedMultiplier * delta;

        if (targetPosition != position)
        {
            position = position + (targetPosition - position) * tPos;
            if (position.DistanceTo(targetPosition) <= 2.0f)
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

    void Camera::HandleEvents(Engine::InputLayer& layer)
    {
        if (layer.Is("gameplay"))
        {
            if (Engine::Input::IsKeyJustPressed(SDLK_LSHIFT))
                moveSpeedMultiplier = 2;
            if (Engine::Input::IsKeyJustReleased(SDLK_LSHIFT))
                moveSpeedMultiplier = 1;

            velocity.x = 0;
            velocity.y = 0;
            if (Engine::Input::IsKeyDown(SDLK_W))
                velocity.y -= 1;
            if (Engine::Input::IsKeyDown(SDLK_S))
                velocity.y += 1;
            if (Engine::Input::IsKeyDown(SDLK_A))
                velocity.x -= 1;
            if (Engine::Input::IsKeyDown(SDLK_D))
                velocity.x += 1;

            if (Engine::Input::IsMouseButtonDown(SDL_BUTTON_MIDDLE))
            {
                Engine::Vec2<float> mouseVel = Engine::Input::GetMouseVelocity();
                targetPosition.x -= mouseVel.x / zoom;
                targetPosition.y -= mouseVel.y / zoom;
                targetPosition = ClampToBounds(targetPosition);
            }

            Engine::Vec2<float> mouseWheelVel = Engine::Input::GetMouseWheelVelocity();
            if (mouseWheelVel.y > 0)
                targetZoom *= 1.1f;
            else if (mouseWheelVel.y < 0)
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

            if (Engine::Input::IsWindowResized())
            {
                Engine::Vec2<int> newViewport = Engine::Window::viewport;

                float newViewportW = newViewport.x;

                float oldZoom = targetZoom;
                float oldViewportW = oldViewport.x;

                float oldVisibleWorldW = oldViewportW / oldZoom;

                float newZoom = newViewportW / oldVisibleWorldW;

                newZoom = std::clamp(newZoom, minZoom, maxZoom);

                targetZoom = newZoom;
                oldViewport = newViewport;
            }
        }
    }
}