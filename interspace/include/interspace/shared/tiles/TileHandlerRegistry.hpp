#pragma once
#include "interspace/shared/tiles/TileHandler.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#define REGISTER_TILE_HANDLER(id, HandlerClass)                                           \
    static bool _registered_##HandlerClass = []()                                         \
    {                                                                                     \
        TileHandlerRegistry::Get().RegisterHandler(id, std::make_unique<HandlerClass>()); \
        return true;                                                                      \
    }()

namespace Interspace
{
    class TileHandlerRegistry
    {
      public:
        static TileHandlerRegistry& Get();
        void RegisterHandler(const std::string& id, std::unique_ptr<TileHandler> handler);
        TileHandler* GetHandler(const std::string& id);

      private:
        std::unordered_map<std::string, std::unique_ptr<TileHandler>> handlers{};
    };
}
