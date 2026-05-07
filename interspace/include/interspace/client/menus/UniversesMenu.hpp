#pragma once

#include <vector>

#include "igneous/scenes/Scene.hpp"

namespace Interspace
{
    struct UniverseEntry
    {
        std::string name;
    };

    class UniversesMenu : public Engine::Scene
    {
      private:
        std::vector<UniverseEntry> universes{};

      public:
        bool isHostingMenu = false;
        std::string prevMenu{};

      private:
        void OnCreated() override;
        void OnActiveChanged(bool value) override;
        void HandleInputs(Engine::InputLayer& layer) override;

      public:
        bool DeleteUniverse(const std::string& universeName);
    };
}