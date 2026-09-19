#pragma once
#include "igneous/engine/Vec2.hpp"
#include "nlohmann/json_fwd.hpp"

namespace Interspace
{
    class TileEntity
    {
      public:
        virtual ~TileEntity() = default;
        virtual void Update(double delta) = 0;
        virtual void Serialize(nlohmann::json& out) const = 0;
        virtual void Deserialize(const nlohmann::json& in) = 0;
        Engine::Vec2<float> pos{};
    };
}
