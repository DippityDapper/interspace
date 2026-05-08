#pragma once
#include "igneous/scenes/Scene.hpp"
#include "steam/steamtypes.h"

namespace Interspace
{
    class SteamManager : public Engine::Scene
    {
      private:
        static inline bool initialized = false;
        static inline bool gameServerInitialized = false;

      public:
        void OnCreated() override;
        void Update(double delta) override;
        void OnDestroyed() override;
 static void Clean();

        static bool TryConnect();
        static bool InitGameServer(uint16 port);

        static void RunCallbacks();

        static bool IsInitialized();
        static bool IsGameServerInitialized();
    };
}