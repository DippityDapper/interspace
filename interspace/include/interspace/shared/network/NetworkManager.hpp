#pragma once

#include <memory>
#include <string>

#include "igneous/scenes/Scene.hpp"
#include "interspace/client/Client.hpp"
#include "interspace/server/Server.hpp"

namespace Interspace
{
    class NetworkManager : public Engine::Scene
    {
      public:
        static constexpr int PORT = 27030;

        static inline std::unique_ptr<Server::Server> server = nullptr;
        static inline std::unique_ptr<Client::Client> client = nullptr;

        /**
         * @brief Polls all active network interfaces each frame.
         */
        void Update(double delta) override;

        /**
         * @brief Tears down all active server and client connections.
         */
        static void Clean();

        // --- Factory methods ---

        /**
         * @brief Creates a remote (ENet) server.
         * @param maxClients Maximum number of simultaneous clients.
         * @param localOnly  If true, binds to 127.0.0.1 only.
         */
        static void CreateServer(int maxClients, bool localOnly = false);

        /**
         * @brief Creates a remote (ENet) client connected to the given IP.
         * @param ip Server IP address.
         */
        static void CreateClient(const std::string& ip);

        /**
         * @brief Creates a Steam-backed server.
         */
        static void CreateSteamServer();

        /**
         * @brief Creates a Steam-backed client connected to the given IP.
         * @param ip Server IP address.
         */
        static void CreateSteamClient(const std::string& ip);

        /**
         * @brief Creates a local loopback server and client pair.
         *
         * Both peers share a loopback connection — no real networking occurs.
         * Useful for singleplayer or local testing.
         */
        static void CreateLocalClientServer();
        /**
         * @brief Creates a local loopback server and client pair backed by ENet.
         *
         * Both peers share a loopback connection — no real networking occurs.
         * Useful for singleplayer or local testing.
         */
        static void CreateRemoteClientServer(int maxClients, bool localOnly = false);

        /**
         * @brief Creates a local loopback server and client pair backed by Steam.
         *
         * Both peers share a loopback connection — no real networking occurs.
         * Useful for testing Steam auth flows locally.
         */
        static void CreateSteamClientServer();
    };
}