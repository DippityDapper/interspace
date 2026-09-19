#pragma once

#include "interspace/shared/tiles/TileEntity.hpp"
#include <memory>

namespace Interspace
{
    class TileHandler
    {
      public:
        virtual ~TileHandler() = default;

        virtual std::unique_ptr<TileEntity> CreateTileEntity()
        {
            return nullptr;
        }
    };
}
