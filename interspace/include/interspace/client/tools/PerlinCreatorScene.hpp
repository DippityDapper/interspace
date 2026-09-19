#pragma once

#include "igneous/engine/PerlinNoise.hpp"
#include "igneous/resources/Sprite.hpp"
#include "igneous/scenes/Scene.hpp"

#include <memory>
#include <string>

namespace Interspace
{
    class PerlinCreatorScene : public Engine::Scene
    {
      private:

        Engine::Perlin perlin;

        Engine::Vec2<float> spritePos{0.0f, 0.0f};
        std::shared_ptr<SDL_Texture> texture;
        Engine::Sprite* sprite = nullptr;

        bool dirty = true;

        char savePath[256] = "assets/perlin/terrain";

        void OnCreated() override;
        void Update(double delta) override;

        void RegenerateTexture();
        void UploadSurface(SDL_Surface* surf);
    };
}
