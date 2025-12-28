#pragma once

#include <memory>

#include "igneous/Camera.hpp"
#include "igneous/Scene.hpp"

namespace Game
{
    class WorldCreationMenu : public Engine::Scene
    {
    private:
        std::unique_ptr<Engine::Camera> camera = nullptr;
        char worldNameLineEdit[16] = "";
        char worldSeedLineEdit[32] = "";
        int worldSize = 0;
        std::string message;

    public:
        std::string prevMenu = "worlds_menu_singleplayer";

    private:
        bool CreateWorld();

    private:
        void Init() override;
        void Render() override;
    };
}
