#pragma once

#include <vector>

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    struct WorldEntry
    {
        std::string name;
    };

    class WorldsMenu : public Engine::Scene
    {
      private:
        std::vector<WorldEntry> worlds{};

      public:
        bool isHostingMenu = false;
        std::string prevMenu{};

      private:
        void Init() override;
        void OnActiveChanged(bool value) override;
        void UI(Engine::InputLayer& layer) override;

      public:
        bool DeleteWorld(const std::string& worldName);
    };
}