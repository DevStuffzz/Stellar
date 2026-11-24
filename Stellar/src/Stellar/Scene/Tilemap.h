#pragma once
#include "Core.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/OrthographicCamera.h"
#include "Stellar/Renderer/TextureAtlas.h"

#include <vector>

namespace Stellar
{
    class ST_API Tilemap
    {
    public:
        static constexpr int EmptyTile = -1;

        // Half-open tile coordinates, clamped to the map.
        struct Bounds
        {
            uint32_t MinX = 0, MinY = 0, MaxX = 0, MaxY = 0;

            bool operator==(const Bounds& other) const
            {
                return MinX == other.MinX && MinY == other.MinY &&
                    MaxX == other.MaxX && MaxY == other.MaxY;
            }
        };

        Tilemap(uint32_t width, uint32_t height);

        // Non-negative tile IDs are atlas sprite IDs; EmptyTile is not rendered.
        int GetTile(uint32_t x, uint32_t y) const;
        void SetTile(uint32_t x, uint32_t y, int tile);

        // Clear() fills the map with EmptyTile.
        void Clear();
        void Clear(int tile);

        int* GetData();
        const int* GetData() const;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        // Returns nullptr for an empty map; regenerate after changing tiles.
        Ref<Mesh> GenerateMesh(const TextureAtlas& atlas, float tileSize = 1.0f) const;
        Ref<Mesh> GenerateMesh(const TextureAtlas& atlas, const Bounds& bounds, float tileSize = 1.0f) const;

        // Includes partially visible tiles. Rotated views use a conservative bounding box.
        Bounds GetVisibleBounds(const OrthographicCamera& camera, float tileSize = 1.0f,
            const glm::mat4& transform = glm::mat4(1.0f)) const;

    private:
        size_t GetIndex(uint32_t x, uint32_t y) const;

        uint32_t m_Width, m_Height;
        std::vector<int> m_Tiles;
    };
}