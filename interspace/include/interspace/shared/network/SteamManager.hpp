#pragma once
#include "steam/steamtypes.h"

namespace Interspace
{
    class SteamManager
    {
      private:
        static inline bool initialized = false;
        static inline bool gameServerInitialized = false;

      public:
        static void Init();
        static void Shutdown();
        static void Clean();

        static bool TryConnect();
        static bool InitGameServer(uint16 port);

        static void RunCallbacks();

        static bool IsInitialized();
        static bool IsGameServerInitialized();
    };
}