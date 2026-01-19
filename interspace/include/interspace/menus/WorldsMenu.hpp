#pragma once

#include <vector>

#include "igneous/scenes/Scene.hpp"

namespace Interspace
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
        void OnActiveChanged(bool value) override;
        void UI(Engine::InputLayer& layer) override;

      public:
        bool DeleteWorld(const std::string& worldName);
    };
}