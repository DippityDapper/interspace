#include "Camera.h"

void Camera::Update(float delta)
{

}

void Camera::HandleEvents(SDL_Event& event)
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
        position.x -= event.motion.xrel / zoom;
        position.y -= event.motion.yrel / zoom;
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
        if (event.wheel.y > 0)
            zoom *= 1.1f;
        else if (event.wheel.y < 0)
            zoom *= 0.9f;

        if (zoom < 0.1f)
            zoom = 0.1f;
        if (zoom > 10.0f)
            zoom = 10.0f;
    }
}
