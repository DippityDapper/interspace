#pragma once

#include <memory>

namespace Interspace
{
    template<typename C>
    concept ExtendsChunk = requires {
        typename C::tile_type;
    } && std::is_base_of_v<Chunk<typename C::tile_type>, C>;

    template<typename chunk_t>
    requires ExtendsChunk<chunk_t>
    class WorldGenerator
    {
      public:
        using TileType = chunk_t::tile_type;

        std::map<Engine::Vec2<uint16_t>, std::unique_ptr<chunk_t>> chunks;
        uint32_t seed = 0;

      public:
        virtual ~WorldGenerator() = default;

        virtual void GenerateChunk(chunk_t* chunk) = 0;
        virtual bool LoadChunkFromDisk(chunk_t* chunk) = 0;
        virtual void SaveChunk(chunk_t* chunk) = 0;
    };
}