#pragma once

#include "igneous/resources/Sprite.hpp"
#include "interspace/shared/world/ChunkData.hpp"

#include <memory>

namespace Interspace::Client
{
    struct ClientChunk
    {
        std::unique_ptr<ChunkData> chunkData = nullptr;
        Engine::Vec2<float> spritePosition{};
        std::unique_ptr<Engine::Sprite> sprite = nullptr;
        std::shared_ptr<SDL_Texture> tileAtlas = nullptr;
    };
}