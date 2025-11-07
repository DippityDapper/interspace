#pragma once

#include <memory>

#include "dapper2d/Scene.hpp"
#include "dapper2d/Sprite.hpp"

#include "dapper2d/Camera.hpp"

namespace Game
{
    class WorldCreationScene : public Engine::Scene
    {
    private:
        std::unique_ptr<Engine::Camera> camera = nullptr;
        char worldName[16];
        char worldSeed[32];
        int worldSize = 0;
        std::string errorMessage{""};

    private:
        bool LoadWorld();
        bool CreateWorld();

    public:
        void Init() override;
        void HandleEvents(SDL_Event& event) override;
        void Update(float delta) override;
        void Render() override;
        void Clean() override;
    };
}