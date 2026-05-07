#include "interspace/client/sounds/SoundManager.hpp"

#include "igneous/resources/ResourceManager.hpp"

namespace Interspace
{
    void SoundManager::Init()
    {
        AddSound("button_1", "assets/sounds/buttons/button_1.mp3", 0);
        AddSound("button_back", "assets/sounds/buttons/button_back.mp3", 0);
    }

    void SoundManager::AddSound(const std::string& name, const std::string& filePath, SDL_PropertiesID props)
    {
        if (sounds.contains(name))
            return;
        auto sound = Engine::ResourceManager::LoadSound(filePath, props);

        if (!sound)
            return;

        sounds.emplace(name, std::move(sound));
    }

    void SoundManager::PlaySound(const std::string& name, float gain)
    {
        if (!sounds.contains(name))
            return;
        auto sound = sounds[name];
        sound->Play(gain);
    }
}
