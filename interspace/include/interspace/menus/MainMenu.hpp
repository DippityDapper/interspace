#pragma once

#include "igneous/Scene.hpp"

namespace Interspace
{
    class MainMenu: public Engine::Scene
    {
    private:
        char usernameLineEdit[16] = "";
        std::string username{};
        std::string errorMessage{};

    private:
        void Init() override;
        void Render() override;

        void CreateSubmenuScenes();
    };
}