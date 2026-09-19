#include "interspace/client/tools/PerlinCreatorScene.hpp"

#include "igneous/resources/ResourceManager.hpp"
#include "imgui.h"

#include <SDL3/SDL.h>
#include <cstdint>
#include <vector>

namespace Interspace
{

    void PerlinCreatorScene::OnCreated()
    {
        perlin.SetSeed(42);

        texture = Engine::ResourceManager::CreateTexture(
                SDL_PIXELFORMAT_RGBA32,
                SDL_TEXTUREACCESS_STREAMING,
                perlin.width,
                perlin.height);

        sprite = new Engine::Sprite(spritePos, texture);
        sprite->centered = false;
    }

    void PerlinCreatorScene::Update(double delta)
    {
        ImGui::Begin("Perlin Noise");

        if (ImGui::SliderFloat("Scale", &perlin.scale, 0.001f, 0.05f, "%.4f")) dirty = true;
        if (ImGui::SliderInt("Octaves", &perlin.octaves, 1, 10)) dirty = true;
        if (ImGui::SliderFloat("Lacunarity", &perlin.lacunarity, 1.0f, 4.0f, "%.2f")) dirty = true;
        if (ImGui::SliderFloat("Gain", &perlin.gain, 0.1f, 0.9f, "%.2f")) dirty = true;

        ImGui::Separator();

        int displaySeed = static_cast<int>(perlin.GetSeed());
        if (ImGui::InputInt("Seed", &displaySeed))
        {
            perlin.SetSeed(static_cast<uint32_t>(displaySeed));
            dirty = true;
        }

        if (ImGui::Button("Randomise Seed"))
        {
            perlin.SetSeed(static_cast<uint32_t>(SDL_GetTicks() & 0x7FFFFFFF));
            dirty = true;
        }

        ImGui::Separator();
        ImGui::Text("Path (no extension)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##path", savePath, sizeof(savePath));

        if (ImGui::Button("Save"))
        {
            std::string pngFilePath(savePath);
            pngFilePath.append(".png");
            perlin.SavePng(pngFilePath);

            std::string dataFilePath(savePath);
            dataFilePath.append(".pnoise");
            perlin.SaveData(dataFilePath);
        }

        ImGui::SameLine();

        if (ImGui::Button("Load"))
        {
            std::string filePath(savePath);
            filePath.append(".pnoise");
            SDL_Surface* surf = perlin.LoadData(filePath);
            if (surf)
            {
                UploadSurface(surf);
                SDL_DestroySurface(surf);
                dirty = false;
            }
        }

        ImGui::End();

        if (dirty)
        {
            RegenerateTexture();
            dirty = false;
        }
    }

    void PerlinCreatorScene::RegenerateTexture()
    {
        std::vector<uint8_t> pixels(perlin.width * perlin.height * 4);

        for (int y = 0; y < perlin.height; y++)
        {
            for (int x = 0; x < perlin.width; x++)
            {
                float noiseValue = perlin.Fbm2_01(static_cast<float>(x),
                                                  static_cast<float>(y));
                uint8_t grey = static_cast<uint8_t>(noiseValue * 255.0f);

                int base = (y * perlin.width + x) * 4;
                pixels[base + 0] = grey;
                pixels[base + 1] = grey;
                pixels[base + 2] = grey;
                pixels[base + 3] = 255;
            }
        }

        SDL_UpdateTexture(texture.get(), nullptr, pixels.data(), perlin.width * 4);
    }

    void PerlinCreatorScene::UploadSurface(SDL_Surface* surf)
    {

        if (surf->w != perlin.width || surf->h != perlin.height)
        {
            texture = Engine::ResourceManager::CreateTexture(
                    SDL_PIXELFORMAT_RGBA32,
                    SDL_TEXTUREACCESS_STREAMING,
                    surf->w,
                    surf->h);

            sprite = new Engine::Sprite(spritePos, texture);
        }

        SDL_UpdateTexture(texture.get(), nullptr, surf->pixels, surf->pitch);
    }

}
