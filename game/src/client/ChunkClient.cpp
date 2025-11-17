#include "game/client/ChunkClient.hpp"

#include "SDL3/SDL_render.h"

#include "dapper2d/Renderer.hpp"
#include "dapper2d/ResourceLoader.hpp"
#include "game/client/TileClient.hpp"

#include "game/client/TileRegistryClient.hpp"
#include "game/client/WorldClient.hpp"
#include "game/network/NetworkPackets.hpp"

namespace Game
{
    ChunkClient::ChunkClient(const Engine::Vec2<uint16_t>& _position, const std::vector<uint8_t>& tileData)
    {
        position = _position;

        bakedTexture = Engine::ResourceLoader::CreateTexture(
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            WorldClient::AREA_SIZE * WorldClient::TILE_SIZE,
            WorldClient::AREA_SIZE * WorldClient::TILE_SIZE
        );

        queuedTileData = tileData;
    }

    void ChunkClient::Init()
    {
    }

    bool ChunkClient::Create()
    {
        size_t offset = 0;

        uint32_t tileCount = UnpackUint32(queuedTileData, offset);

        uint32_t queueCount = 0;
        while (queueCount < tileCount)
        {
            uint8_t tileX = queueCount % WorldClient::AREA_SIZE;
            uint8_t tileY = queueCount / WorldClient::AREA_SIZE;

            TileType tileType = static_cast<TileType>(UnpackUint8(queuedTileData, offset));

            Engine::Vec2<uint8_t> tilePosition = {tileX, tileY};
            TileClient* tile = TileRegistryClient::GetTile(tileType);
            tiles[tilePosition] = tile;
            queueCount++;
        }

        queuedTileData.clear();
        return true;
    }

    void ChunkClient::BakeSprite() const
    {
        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), bakedTexture.get());

        for (const auto& kvp : tiles)
        {
            TileClient* tile = kvp.second;
            tile->LocalRender(Engine::Vec2<float>(kvp.first));
        }

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void ChunkClient::Update(float delta)
    {
        if (fogAlpha < 1.0f)
            fogAlpha += delta / fadeInSpeed;
    }

    void ChunkClient::Clean()
    {
    }

    void ChunkClient::Render()
    {
        if (!bakedTexture)
            return;

        if (tiles.size() >= WorldClient::AREA_SIZE * WorldClient::AREA_SIZE)
        {
            if (fogAlpha < 1.0f)
            {
                SDL_SetTextureAlphaMod(bakedTexture.get(), static_cast<Uint8>(fogAlpha * 255));
            }
            else if (fogAlpha > 1.0f)
            {
                SDL_SetTextureAlphaMod(bakedTexture.get(), 255);
                fogAlpha = 1.0f;
            }

            Engine::Vec2<float> areaPosition = (Engine::Vec2<float>)position * WorldClient::AREA_SIZE * WorldClient::TILE_SIZE;
            Engine::Renderer::BufferAdd(areaPosition, bakedTexture.get(), nullptr, false);
        }
    }
}
