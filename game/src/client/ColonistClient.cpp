#include "game/client/ColonistClient.hpp"

namespace Game
{
    ColonistClient::ColonistClient(uint32_t _id, const std::string& _name, const Engine::Vec2<float>& _position)
    {
        id = _id;
        name = _name;
        position = _position;
    }
}
