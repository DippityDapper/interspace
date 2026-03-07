#include "interspace/client/ClientTiles.hpp"

namespace Interspace::Client
{
    void ClientTiles::Init()
    {
        SQLite::Database* commonDb = DBHelper::commonDb.get();

        SQLite::Statement statement(*commonDb, R"(
            SELECT * FROM tileData
        )");

        while (statement.executeStep())
        {
            uint32_t tileId = statement.getColumn(0).getUInt();
            uint32_t tileVariant = statement.getColumn(1).getUInt();
            std::string tileName = statement.getColumn(2).getString();
            bool walkable = statement.getColumn(3).getUInt();
            std::string tileTexturePath = statement.getColumn(4).getString();
            uint32_t tileAtlasWidth = statement.getColumn(5).getUInt();
            uint32_t tileAtlasHeight = statement.getColumn(6).getUInt();
            uint32_t tileAtlasX = statement.getColumn(7).getUInt();
            uint32_t tileAtlasY = statement.getColumn(8).getUInt();

            std::unique_ptr<ClientTile> tile = std::make_unique<ClientTile>();
            tile->tileId = tileId;
            tile->variant = tileVariant;
            tile->tileName = tileName;
            tile->walkable = walkable;

            Engine::Vec2<float> dummyPosition{};
            tile->sprite = std::make_unique<Engine::Sprite>(
                    dummyPosition,
                    tileTexturePath,
                    tileAtlasWidth,
                    tileAtlasHeight,
                    tileAtlasX,
                    tileAtlasY);

            tile->sprite->render = false;
            tile->sprite->centered = false;

            tiles[tileId][tileVariant] = std::move(tile);
        }
    }
}
