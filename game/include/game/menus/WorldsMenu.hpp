#pragma once

#include <vector>

#include "dapper2d/Scene.hpp"

namespace Game
{
    class WorldsMenu : public Engine::Scene
    {
    private:
        std::vector<std::string> worlds{};

    public:
        bool isHostingMenu = false;
        std::string prevMenu = "";

    private:
        void Init() override;
        // void Update(float delta) override;
        void Render() override;
        // void HandleEvents(SDL_Event& event) override;
        // void Clean() override;

    public:
        // bool LoadWorld(const std::string& worldName);
        bool DeleteWorld(const std::string& worldName);
    };
}