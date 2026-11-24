#include "Stellar/Scene/Tilemap.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace Stellar
{
    Tilemap::Tilemap(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height),
          m_Tiles(static_cast<size_t>(width) * height, EmptyTile)
    {
    }

    int Tilemap::GetTile(uint32_t x, uint32_t y) const
    {
        ST_ASSERT(x < m_Width && y < m_Height, "Tilemap coordinates are out of bounds!");
        return m_Tiles[GetIndex(x, y)];
    }

    void Tilemap::SetTile(uint32_t x, uint32_t y, int tile)
    {
        ST_ASSERT(x < m_Width && y < m_Height, "Tilemap coordinates are out of bounds!");
        ST_ASSERT(tile >= EmptyTile, "Tilemap IDs must be -1 or non-negative!");
        m_Tiles[GetIndex(x, y)] = tile;
    }

    void Tilemap::SetTileCollision(int tile, bool solid)
    {
        ST_ASSERT(tile >= 0, "Empty or negative tiles cannot be solid!");
        if (solid)
            m_SolidTiles.insert(tile);
        else
            m_SolidTiles.erase(tile);
    }

    bool Tilemap::IsTileSolid(int tile) const
    {
        return tile != EmptyTile && m_SolidTiles.count(tile) != 0;
    }

    glm::vec2 Tilemap::MoveWithCollisions(const glm::vec2& position, const glm::vec2& halfSize,
        const glm::vec2& displacement, float tileSize, const glm::vec2& mapCenter) const
    {
        ST_ASSERT(tileSize > 0.0f, "Tile size must be positive!");
        ST_ASSERT(halfSize.x > 0.0f && halfSize.y > 0.0f, "Collider half-size must be positive!");

        const glm::vec2 origin = mapCenter - glm::vec2(
            static_cast<float>(m_Width) * tileSize, static_cast<float>(m_Height) * tileSize) * 0.5f;
        const auto clampIndex = [](double index, uint32_t limit) {
            return static_cast<uint32_t>(std::clamp(index, 0.0, static_cast<double>(limit)));
        };
        glm::vec2 result = position;

        for (int axis = 0; axis < 2; axis++)
        {
            const float distance = displacement[axis];
            if (distance == 0.0f)
                continue;

            glm::vec2 end = result;
            end[axis] += distance;
            const glm::vec2 minPoint = glm::min(result, end) - halfSize;
            const glm::vec2 maxPoint = glm::max(result, end) + halfSize;
            const uint32_t minX = clampIndex(std::floor((minPoint.x - origin.x) / tileSize), m_Width);
            const uint32_t maxX = clampIndex(std::ceil((maxPoint.x - origin.x) / tileSize), m_Width);
            const uint32_t minY = clampIndex(std::floor((minPoint.y - origin.y) / tileSize), m_Height);
            const uint32_t maxY = clampIndex(std::ceil((maxPoint.y - origin.y) / tileSize), m_Height);

            for (uint32_t y = minY; y < maxY; y++)
            {
                for (uint32_t x = minX; x < maxX; x++)
                {
                    if (!IsTileSolid(GetTile(x, y)))
                        continue;

                    const glm::vec2 tileMin = origin + glm::vec2(x, y) * tileSize;
                    const glm::vec2 tileMax = tileMin + glm::vec2(tileSize);
                    const int otherAxis = 1 - axis;
                    if (result[otherAxis] <= tileMin[otherAxis] - halfSize[otherAxis] ||
                        result[otherAxis] >= tileMax[otherAxis] + halfSize[otherAxis])
                        continue;

                    if (distance > 0.0f && result[axis] <= tileMin[axis] - halfSize[axis])
                        end[axis] = std::min(end[axis], tileMin[axis] - halfSize[axis]);
                    else if (distance < 0.0f && result[axis] >= tileMax[axis] + halfSize[axis])
                        end[axis] = std::max(end[axis], tileMax[axis] + halfSize[axis]);
                }
            }

            result[axis] = end[axis];
        }
        return result;
    }

    void Tilemap::Clear()
    {
        Clear(EmptyTile);
    }

    void Tilemap::Clear(int tile)
    {
        ST_ASSERT(tile >= EmptyTile, "Tilemap IDs must be -1 or non-negative!");
        std::fill(m_Tiles.begin(), m_Tiles.end(), tile);
    }

    int* Tilemap::GetData()
    {
        return m_Tiles.data();
    }

    const int* Tilemap::GetData() const
    {
        return m_Tiles.data();
    }

    Ref<Mesh> Tilemap::GenerateMesh(const TextureAtlas& atlas, float tileSize) const
    {
        return GenerateMesh(atlas, { 0, 0, m_Width, m_Height }, tileSize);
    }

    Tilemap::Bounds Tilemap::GetVisibleBounds(const OrthographicCamera& camera, float tileSize,
        const glm::mat4& transform) const
    {
        ST_ASSERT(tileSize > 0.0f, "Tile size must be positive!");
        const glm::mat4 clipToMap = glm::inverse(camera.GetViewProjectionMatrix() * transform);
        glm::vec2 minPoint(std::numeric_limits<float>::max());
        glm::vec2 maxPoint(std::numeric_limits<float>::lowest());

        for (int y = -1; y <= 1; y += 2)
        {
            for (int x = -1; x <= 1; x += 2)
            {
                const glm::vec4 point = clipToMap * glm::vec4(static_cast<float>(x), static_cast<float>(y), 0.0f, 1.0f);
                const glm::vec2 local = glm::vec2(point) / point.w;
                minPoint = glm::min(minPoint, local);
                maxPoint = glm::max(maxPoint, local);
            }
        }

        const double originX = -static_cast<double>(m_Width) * tileSize * 0.5;
        const double originY = -static_cast<double>(m_Height) * tileSize * 0.5;
        const auto clampIndex = [](double index, uint32_t limit) {
            return static_cast<uint32_t>(std::clamp(index, 0.0, static_cast<double>(limit)));
        };
        return {
            clampIndex(std::floor((minPoint.x - originX) / tileSize), m_Width),
            clampIndex(std::floor((minPoint.y - originY) / tileSize), m_Height),
            clampIndex(std::ceil((maxPoint.x - originX) / tileSize), m_Width),
            clampIndex(std::ceil((maxPoint.y - originY) / tileSize), m_Height)
        };
    }

    Ref<Mesh> Tilemap::GenerateMesh(const TextureAtlas& atlas, const Bounds& bounds, float tileSize) const
    {
        ST_ASSERT(tileSize > 0.0f, "Tile size must be positive!");
        ST_ASSERT(atlas.GetTexture(), "Tilemap texture atlas must contain a texture!");
        ST_ASSERT(bounds.MinX <= bounds.MaxX && bounds.MaxX <= m_Width &&
            bounds.MinY <= bounds.MaxY && bounds.MaxY <= m_Height, "Tilemap mesh bounds are invalid!");

        std::vector<Vertex2D> vertices;
        std::vector<uint32_t> indices;
        const size_t tileCount = static_cast<size_t>(bounds.MaxX - bounds.MinX) * (bounds.MaxY - bounds.MinY);
        vertices.reserve(tileCount * 4);
        indices.reserve(tileCount * 6);

        const float mapWidth = static_cast<float>(m_Width) * tileSize;
        const float mapHeight = static_cast<float>(m_Height) * tileSize;
        const float originX = -mapWidth * 0.5f;
        const float originY = -mapHeight * 0.5f;

        for (uint32_t y = bounds.MinY; y < bounds.MaxY; y++)
        {
            for (uint32_t x = bounds.MinX; x < bounds.MaxX; x++)
            {
                const int tileId = GetTile(x, y);
                if (tileId == EmptyTile)
                    continue;

                const uint32_t atlasId = static_cast<uint32_t>(tileId);
                ST_ASSERT(atlasId < atlas.GetSubTextureCount(), "Tilemap texture ID is outside the atlas!");
                const std::array<glm::vec2, 4> texCoords = atlas.GetTexCoords(atlasId);

                const float left = originX + static_cast<float>(x) * tileSize;
                const float bottom = originY + static_cast<float>(y) * tileSize;
                const float right = left + tileSize;
                const float top = bottom + tileSize;

                const uint32_t base = static_cast<uint32_t>(vertices.size());
                vertices.push_back({ { left,  bottom }, glm::vec4(1.0f), texCoords[0] });
                vertices.push_back({ { right, bottom }, glm::vec4(1.0f), texCoords[1] });
                vertices.push_back({ { right, top },    glm::vec4(1.0f), texCoords[2] });
                vertices.push_back({ { left,  top },    glm::vec4(1.0f), texCoords[3] });

                indices.push_back(base);
                indices.push_back(base + 1);
                indices.push_back(base + 2);
                indices.push_back(base + 2);
                indices.push_back(base + 3);
                indices.push_back(base);
            }
        }

        if (vertices.empty())
            return nullptr;

        return CreateRef<Mesh>(vertices, indices);
    }

    size_t Tilemap::GetIndex(uint32_t x, uint32_t y) const
    {
        return static_cast<size_t>(y) * m_Width + x;
    }
}
