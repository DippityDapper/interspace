#pragma once
#include <map>
#include <memory>
#include <string>

#include "igneous/AudioStream.hpp"

#ifdef PlaySound
#undef PlaySound
#endif

namespace Game
{
    class Sounds
    {
    private:
        static inline std::map<std::string, std::shared_ptr<Engine::AudioStream>> sounds{};

    public:
        static void AddSound(const std::string& name, const std::string& filePath, SDL_PropertiesID props);
        static void PlaySound(const std::string& name, float gain);
    };
}
