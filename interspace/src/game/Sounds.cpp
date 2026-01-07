#include "interspace/game/Sounds.hpp"

#include "igneous/ResourceLoader.hpp"

namespace Interspace
{
    void Sounds::AddSound(const std::string& name, const std::string& filePath, SDL_PropertiesID props)
    {
        if (sounds.contains(name))
            return;
        auto sound = Engine::ResourceLoader::LoadSound(filePath, props);

        if (!sound)
            return;

        sounds.emplace(name, std::move(sound));
    }

    void Sounds::PlaySound(const std::string& name, float gain)
    {
        if (!sounds.contains(name))
            return;
        auto sound = sounds[name];
        sound->Play(gain);
    }
}
