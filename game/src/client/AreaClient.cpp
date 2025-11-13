#include "game/client/AreaClient.hpp"

#include "SDL3/SDL_render.h"

#include "dapper2d/Renderer.hpp"
#include "dapper2d/ResourceLoader.hpp"

#include "game/client/TileRegistryClient.hpp"
#include "game/client/WorldClient.hpp"
#include "game/network/NetworkPackets.hpp"

namespace Game
{
    AreaClient::AreaClient(const Engine::Vec2<uint16_t>& _position, const std::vector<uint8_t>& tileData)
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

    void AreaClient::Create()
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
    }

    void AreaClient::BakeSprite()
    {
        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), bakedTexture.get());

        for (const auto& tile : tiles)
        {
            tile.second->LocalRender(Engine::Vec2<float>(tile.first));
        }

        SDL_SetRenderTarget(Engine::Renderer::GetRenderer(), nullptr);
    }

    void AreaClient::Update(float delta)
    {

    }

    void AreaClient::Render()
    {
        if (!bakedTexture)
            return;

        if (tiles.size() >= WorldClient::AREA_SIZE * WorldClient::AREA_SIZE)
        {
            // SDL_SetTextureAlphaMod(cachedTexture.get(), static_cast<Uint8>(fogAlpha * 255));
            Engine::Vec2<float> areaPosition = (Engine::Vec2<float>)position * WorldClient::AREA_SIZE * WorldClient::TILE_SIZE;
            Engine::Renderer::BufferAdd(areaPosition, bakedTexture.get(), nullptr, false);
        }
    }
}
