#include "game/World.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "imgui.h"
#include "SDL3/SDL.h"

#include "dapper2d/CFGParser.hpp"
#include "dapper2d/Engine.hpp"
#include "dapper2d/Input.hpp"
#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Window.hpp"

#include "game/Tile.hpp"

namespace Game
{
    uint32_t World::worldSeed = 0;
    int World::WORLD_SIZE_X = 0;
    int World::WORLD_SIZE_Y = 0;
    int World::REGION_SIZE = 32;

    World::World(const std::string &worldName)
    {
        name = worldName;
        Engine::CFGParser::LoadConfig("worlds/" + worldName + "/configs.cfg", worldName);

        try
        {
            worldSeed = Engine::CFGParser::GetUInt32(worldName, "world_seed");
            WORLD_SIZE_X = Engine::CFGParser::GetInt(worldName, "world_size_x");
            WORLD_SIZE_Y = Engine::CFGParser::GetInt(worldName, "world_size_y");
            Area::AREA_SIZE = Engine::CFGParser::GetInt(worldName, "area_size");
            Tile::TILE_SIZE = Engine::CFGParser::GetInt(worldName, "tile_size");
        }
        catch (const std::exception& e)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", e.what());
            Engine::Engine::Quit();
            return;
        }
    }

    void World::Init()
    {
        Engine::ResourceLoader::SetScaleMode(SDL_SCALEMODE_PIXELART);
//        SDL_SetWindowFullscreen(Engine::Window::GetWindow(), true);
        Tiles::InitRegistry();

        camera = std::make_unique<Camera>
        (
            (WORLD_SIZE_X * Area::AREA_SIZE * Tile::TILE_SIZE) / 2.0f,
            (WORLD_SIZE_Y * Area::AREA_SIZE * Tile::TILE_SIZE) / 2.0f,
            1.0f
        );
        camera->SetCurrent();

        camera->minZoom = 0.05;
        camera->limitBounds = true;
        camera->limitLeft = 0.0f;
        camera->limitRight = WORLD_SIZE_X * Area::AREA_SIZE * Tile::TILE_SIZE;
        camera->limitTop = 0.0f;
        camera->limitBottom = WORLD_SIZE_Y * Area::AREA_SIZE * Tile::TILE_SIZE;
    }

    void World::HandleEvents(SDL_Event &event)
    {
        if (event.key.down)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                Engine::Engine::Quit();
                return;
            }
        }
        if (Input::IsKeyDown(SDLK_E))
        {
            float mX = 0;
            float mY = 0;
            SDL_GetMouseState(&mX, &mY);

            Engine::Vec2<float> mouseLocalPosition{mX, mY};
            Engine::Vec2<float> viewportOffset = (Engine::Vec2<float>)Engine::Window::viewport / 2.0f;

            Engine::Vec2<float> mouseGlobalPosition = camera->position + ((mouseLocalPosition - viewportOffset) / camera->zoom);
            Engine::Vec2<int> mouseTilePosition = (mouseGlobalPosition / Tile::TILE_SIZE).Floor();

            Engine::Vec2<int> mouseAreaPosition = mouseTilePosition / Area::AREA_SIZE;
            Engine::Vec2<int> mouseLocalTilePosition = mouseTilePosition % Area::AREA_SIZE;

            if (areas.contains(mouseAreaPosition))
            {
                Area* area = areas[mouseAreaPosition].get();
                area->UpdateTile(mouseLocalTilePosition, STONE_PATH);
            }
        }
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_F)
            {
                SaveWorld();
            }
        }
    }

    void World::Update(float delta)
    {
        Engine::Vec2<float> cameraPosition = Engine::Camera::main->position;
        float minZoom = std::max(camera->zoom, minZoomForRendering);

        Engine::Vec2<float> viewportOffset = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / minZoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();

        int areasAdded = 0;

        for (int y = minBounds.y-1; y <= maxBounds.y+1; ++y)
        {
            for (int x = minBounds.x-1; x <= maxBounds.x+1; ++x)
            {
                Engine::Vec2<int> visibleAreaPosition{x, y};
                if (areas.contains(visibleAreaPosition))
                {
                    areas[visibleAreaPosition]->Update(delta);
                }
                else if (areasAdded <= 1 && !areas.contains(visibleAreaPosition) && x >= 0 && x < WORLD_SIZE_X && y >= 0 && y < WORLD_SIZE_Y)
                {
                    std::unique_ptr<Game::Area> loadedArea = LoadAreaFromRegion(x, y, REGION_SIZE);

                    if (loadedArea)
                    {
                        areas[visibleAreaPosition] = std::move(loadedArea);
                        areas[visibleAreaPosition]->AddTilesToTexture();
                    }
                    else
                    {
                        areas[visibleAreaPosition] = std::make_unique<Game::Area>(x, y);
                        areas[visibleAreaPosition]->GenerateTiles();
                    }

                    areas[visibleAreaPosition]->Update(delta);
                    areasAdded++;
                }
            }
        }

        for (auto it = areas.begin(); it != areas.end();)
        {
            const Engine::Vec2<int>& pos = it->first;

            if (pos.x < minBounds.x - 1 || pos.x > maxBounds.x + 1 ||
                pos.y < minBounds.y - 1 || pos.y > maxBounds.y + 1)
            {
                it = areas.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void World::Render()
    {
        RenderAreas();

        ImGui::SetNextWindowPos({0,0});
        ImGui::Begin("Debug");

        ImGui::Text("Seed : %zu", worldSeed);

        ImGui::Text("Rendered Area Count : %d", areas.size());
        ImGui::SliderFloat("Min zoom for rendering", &minZoomForRendering, Camera::main->minZoom, Camera::main->maxZoom);
        ImGui::SliderFloat("Camera Speed", &camera->moveSpeed, 1, 100);

        Engine::Vec2<int> cameraTilePosition = (Engine::Camera::main->position / Tile::TILE_SIZE).Floor();
        Engine::Vec2<int> cameraAreaPosition = cameraTilePosition / Area::AREA_SIZE;
        ImGui::Text("Camera Area Position : (%d, %d)", cameraAreaPosition.x, cameraAreaPosition.y);

        Engine::Vec2<int> cameraLocalTilePosition = cameraTilePosition % Area::AREA_SIZE;
        ImGui::Text("Camera Local Tile Position : (%d, %d)", cameraLocalTilePosition.x, cameraLocalTilePosition.y);

        ImGui::Text("Camera Zoom : %.4f", camera->zoom);

        float mX = 0;
        float mY = 0;
        SDL_GetMouseState(&mX, &mY);

        Engine::Vec2<float> mouseLocalPosition{mX, mY};
        Engine::Vec2<float> viewportOffset = (Engine::Vec2<float>)Engine::Window::viewport / 2.0f;

        Engine::Vec2<float> mouseGlobalPosition = camera->position + ((mouseLocalPosition - viewportOffset) / camera->zoom);
        Engine::Vec2<int> mouseTilePosition = (mouseGlobalPosition / Tile::TILE_SIZE).Floor();

        Engine::Vec2<int> mouseAreaPosition = mouseTilePosition / Area::AREA_SIZE;
        ImGui::Text("Mouse Area Position : (%d, %d)", mouseAreaPosition.x, mouseAreaPosition.y);

        Engine::Vec2<int> mouseLocalTilePosition = mouseTilePosition % Area::AREA_SIZE;
        ImGui::Text("Mouse Local Tile Position : (%d, %d)", mouseLocalTilePosition.x, mouseLocalTilePosition.y);

        Engine::Camera* camera = Engine::Camera::main;
        float zoom = camera->zoom;

        Engine::Vec2<float> cameraPosition = camera->position;
        Engine::Vec2<float> viewportOffset2 = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / zoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset2) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset2) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();

        ImGui::Text("Rendered X chunks : (%d, %d)", minBounds.x, maxBounds.x);
        ImGui::Text("Rendered Y chunks : (%d, %d)", minBounds.y, maxBounds.y);

        ImGui::Text("Viewport Size : (%d, %d)", Engine::Window::viewport.x, Engine::Window::viewport.y);

        ImGui::End();
    }

    void World::RenderAreas()
    {
        float zoom = camera->zoom;

        Engine::Vec2<float> cameraPosition = camera->position;
        Engine::Vec2<float> viewportOffset = ((Engine::Vec2<float>)Engine::Window::viewport  / 2.0f) / zoom;

        Engine::Vec2<int> minBounds = ((cameraPosition - viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();
        Engine::Vec2<int> maxBounds = ((cameraPosition + viewportOffset) / (Tile::TILE_SIZE * Area::AREA_SIZE)).Floor();

        for (int y = minBounds.y; y <= maxBounds.y; ++y)
        {
            for (int x = minBounds.x; x <= maxBounds.x; ++x)
            {
                Engine::Vec2<int> visibleAreaPosition{x, y};
                if (areas.contains(visibleAreaPosition))
                {
                    areas[visibleAreaPosition]->Render();
                }
            }
        }
    }

    void World::RenderEntities()
    {
    }

    void World::Clean()
    {
        areas.clear();
    }

    void World::SaveWorld()
    {
        if (!std::filesystem::exists("worlds"))
            std::filesystem::create_directories("worlds");
        if (!std::filesystem::exists("worlds/" + name))
            std::filesystem::create_directories("worlds/" + name);
        if (!std::filesystem::exists("worlds/" + name + "/regions"))
            std::filesystem::create_directories("worlds/" + name + "/regions");

        for (auto& [areaPos, areaPtr] : areas)
        {
            if (!areaPtr->needsSave)
                continue;

            int rx = areaPos.x / REGION_SIZE;
            int ry = areaPos.y / REGION_SIZE;
            std::string filePath = "worlds/" + name + "/regions/region_" + std::to_string(rx) + "_" + std::to_string(ry) + ".data";

            SaveAreaToRegion(areaPtr.get(), rx, ry, filePath);
            SDL_Log("Saved area (%d, %d) to region file %s", areaPos.x, areaPos.y, filePath.c_str());
            areaPtr->needsSave = false;
        }
    }


    void World::SaveAreaToRegion(Game::Area* area, int rx, int ry, const std::string& filePath)
    {
        // Step 1: Read existing region data
        std::vector<std::tuple<int,int,std::vector<std::tuple<int,int,int>>>> regionAreas;

        if (FileExists(filePath))
        {
            std::ifstream infile(filePath, std::ios::in | std::ios::binary);
            while (infile.peek() != EOF)
            {
                int savedAreaX, savedAreaY, tileCount;
                infile.read(reinterpret_cast<char*>(&savedAreaX), sizeof(int));
                infile.read(reinterpret_cast<char*>(&savedAreaY), sizeof(int));
                infile.read(reinterpret_cast<char*>(&tileCount), sizeof(int));

                std::vector<std::tuple<int,int,int>> tiles(tileCount);
                for (int i = 0; i < tileCount; ++i)
                {
                    int tx, ty, tType;
                    infile.read(reinterpret_cast<char*>(&tx), sizeof(int));
                    infile.read(reinterpret_cast<char*>(&ty), sizeof(int));
                    infile.read(reinterpret_cast<char*>(&tType), sizeof(int));
                    tiles[i] = {tx, ty, tType};
                }

                regionAreas.push_back({savedAreaX, savedAreaY, tiles});
            }
            infile.close();
        }

        // Step 2: Replace or append the area
        auto it = std::find_if(regionAreas.begin(), regionAreas.end(),
                               [&](auto& a){ return std::get<0>(a) == area->position.x &&
                                                    std::get<1>(a) == area->position.y; });
        std::vector<std::tuple<int,int,int>> newTiles;
        newTiles.reserve(area->tiles.size());
        for (auto& tileKvp : area->tiles)
        {
            auto tile = tileKvp.second;
            newTiles.push_back({tileKvp.first.x, tileKvp.first.y, static_cast<int>(tile->type)});
        }

        if (it != regionAreas.end())
            *it = {area->position.x, area->position.y, newTiles};
        else
            regionAreas.push_back({area->position.x, area->position.y, newTiles});

        // Step 3: Write everything back cleanly
        std::ofstream outfile(filePath, std::ios::out | std::ios::binary | std::ios::trunc);

        for (auto& [ax, ay, tiles] : regionAreas)
        {
            int tileCount = static_cast<int>(tiles.size());
            outfile.write(reinterpret_cast<char*>(&ax), sizeof(int));
            outfile.write(reinterpret_cast<char*>(&ay), sizeof(int));
            outfile.write(reinterpret_cast<char*>(&tileCount), sizeof(int));

            for (auto& [tx, ty, tType] : tiles)
            {
                outfile.write(reinterpret_cast<char*>(&tx), sizeof(int));
                outfile.write(reinterpret_cast<char*>(&ty), sizeof(int));
                outfile.write(reinterpret_cast<char*>(&tType), sizeof(int));
            }
        }

        outfile.close();
    }

    bool World::FileExists(const std::string& fileName)
    {
        std::ifstream infile(fileName, std::ios::binary);
        return infile.good();
    }

    std::unique_ptr<Game::Area> World::LoadAreaFromRegion(int areaX, int areaY, int regionSize)
    {
        int rx = areaX / regionSize;
        int ry = areaY / regionSize;
        std::string fileName = "region_" + std::to_string(rx) + "_" + std::to_string(ry) + ".data";
        std::string filePath = "worlds/" + name + "/regions/" + fileName;

        if (!FileExists(filePath))
            return nullptr;

        std::ifstream infile(filePath, std::ios::in | std::ios::binary);
        if (!infile.is_open())
            return nullptr;

        while (infile.peek() != EOF)
        {
            int savedAreaX, savedAreaY;
            infile.read(reinterpret_cast<char*>(&savedAreaX), sizeof(int));
            infile.read(reinterpret_cast<char*>(&savedAreaY), sizeof(int));

            int tileCount;
            infile.read(reinterpret_cast<char*>(&tileCount), sizeof(int));

            if (savedAreaX == areaX && savedAreaY == areaY)
            {
                auto area = std::make_unique<Game::Area>(areaX, areaY);
                area->tiles.clear();

                for (int i = 0; i < tileCount; ++i)
                {
                    int tx, ty, tileType;
                    infile.read(reinterpret_cast<char*>(&tx), sizeof(int));
                    infile.read(reinterpret_cast<char*>(&ty), sizeof(int));
                    infile.read(reinterpret_cast<char*>(&tileType), sizeof(int));

                    Game::Tile* tile = Tiles::GetTile(static_cast<TileType>(tileType));
                    area->tiles[{tx, ty}] = tile;
                }

                infile.close();
                return area;
            }
            else
            {
                infile.seekg(tileCount * (sizeof(int) * 3), std::ios::cur);
            }
        }

        infile.close();
        return nullptr;
    }
}
