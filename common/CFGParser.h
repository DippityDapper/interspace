#ifndef SDL3_FIRST_PROJECT_CFGPARSER_H
#define SDL3_FIRST_PROJECT_CFGPARSER_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <cstdlib>
#include "SDL3/SDL.h"

class CFGParser
{
private:
    static std::map<std::string, std::map<std::string, std::string>> configs;

public:
    static void LoadConfig(const char* fileName);
    static const char* GetString(const char* configName, const char* config);
    static int GetInt(const char* configName, const char* config);
};


#endif
