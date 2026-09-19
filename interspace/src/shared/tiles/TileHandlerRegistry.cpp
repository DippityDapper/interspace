#include "interspace/shared/tiles/TileHandlerRegistry.hpp"

namespace Interspace
{
    TileHandlerRegistry& TileHandlerRegistry::Get()
    {
        static TileHandlerRegistry instance;
        return instance;
    }
}
