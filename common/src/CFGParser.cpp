#include "CFGParser.h"

std::map<std::string, std::map<std::string, std::string>> CFGParser::configs;

void CFGParser::LoadConfig(const char* fileName)
{
    char *fullPath = nullptr;
    SDL_asprintf(&fullPath, "%sconfigs/%s", SDL_GetBasePath(), fileName);

    std::ifstream serverPropertiesFile(fullPath);
    SDL_free(fullPath);

    if (!serverPropertiesFile.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load server config file");
        return;
    }

    configs[fileName] = {};
    std::string line;

    while (std::getline(serverPropertiesFile, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        auto delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos)
            continue;

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        configs[fileName][key] = value;
    }
}

const char* CFGParser::GetString(const char* configName, const char* key)
{
    auto fileIt = configs.find(configName);
    if (fileIt == configs.end())
        return "";

    auto& kvp = fileIt->second;
    auto it = kvp.find(key);
    if (it == kvp.end())
        return "";

    return it->second.c_str();
}

int CFGParser::GetInt(const char* configName, const char* key)
{
    auto fileIt = configs.find(configName);
    if (fileIt == configs.end())
        return 0;

    auto& kvp = fileIt->second;
    auto it = kvp.find(key);
    if (it == kvp.end())
        return 0;

    return std::stoi(it->second);
}
