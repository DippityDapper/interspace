#pragma once

#include <memory>

#include "dapper2d/Camera.hpp"
#include "dapper2d/Scene.hpp"

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
        bool LoadWorld();
        bool CreateWorld();

        bool HostWorld();
        bool StopHostingWorld();

        bool ConnectToWorld();
        bool DisconnectFromWorld();

    private:
        void Init() override;
        // void HandleEvents(SDL_Event& event) override;
        // void Update(float delta) override;
        void Render() override;
        // void Clean() override;
    };
}
