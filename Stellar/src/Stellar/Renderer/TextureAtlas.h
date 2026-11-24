#pragma once

#include "Core.h"
#include "Stellar/Renderer/Texture.h"

#include <array>

#include <glm/glm.hpp>

namespace Stellar {

    // Fixed-size grid atlas. Sub-textures are addressed by id in row-major
    // order starting at the top-left cell: in a 2x2 grid, 0 is the top-left,
    // 1 the top-right, 2 the bottom-left and 3 the bottom-right.
    class ST_API TextureAtlas
    {
    public:
        TextureAtlas(uint32_t textureWidth, uint32_t textureHeight, uint32_t cellWidth, uint32_t cellHeight)
            : m_TextureWidth(textureWidth), m_TextureHeight(textureHeight),
              m_CellWidth(cellWidth), m_CellHeight(cellHeight)
        {
            ST_ASSERT(cellWidth > 0 && cellHeight > 0, "Atlas cell size must be non-zero!");
            m_Columns = textureWidth / cellWidth;
            m_Rows = textureHeight / cellHeight;
            ST_ASSERT(m_Columns > 0 && m_Rows > 0, "Atlas cell size is larger than the texture!");
        }

        TextureAtlas(const Ref<Texture2D>& texture, uint32_t cellWidth, uint32_t cellHeight)
            : TextureAtlas(texture->GetWidth(), texture->GetHeight(), cellWidth, cellHeight)
        {
            m_Texture = texture;
        }

        uint32_t GetColumns() const { return m_Columns; }
        uint32_t GetRows() const { return m_Rows; }
        uint32_t GetSubTextureCount() const { return m_Columns * m_Rows; }
        uint32_t GetTextureWidth() const { return m_TextureWidth; }
        uint32_t GetTextureHeight() const { return m_TextureHeight; }
        uint32_t GetCellWidth() const { return m_CellWidth; }
        uint32_t GetCellHeight() const { return m_CellHeight; }
        const Ref<Texture2D>& GetTexture() const { return m_Texture; }

        // Pixel coordinates of the sub-texture's top-left corner (y down from the image top).
        glm::uvec2 GetCellCoords(uint32_t id) const
        {
            ST_ASSERT(id < GetSubTextureCount(), "Sub-texture id is out of range!");
            return { (id % m_Columns) * m_CellWidth, (id / m_Columns) * m_CellHeight };
        }

        // Bottom-left UV of the sub-texture. Images load bottom-up, so the
        // top-left cell (id 0) sits at the top of the UV range.
        glm::vec2 GetUVMin(uint32_t id) const
        {
            ST_ASSERT(id < GetSubTextureCount(), "Sub-texture id is out of range!");
            uint32_t column = id % m_Columns;
            uint32_t row = id / m_Columns;
            return {
                static_cast<float>(column * m_CellWidth) / static_cast<float>(m_TextureWidth),
                static_cast<float>(m_TextureHeight - (row + 1) * m_CellHeight) / static_cast<float>(m_TextureHeight)
            };
        }

        // Top-right UV of the sub-texture.
        glm::vec2 GetUVMax(uint32_t id) const
        {
            ST_ASSERT(id < GetSubTextureCount(), "Sub-texture id is out of range!");
            uint32_t column = id % m_Columns;
            uint32_t row = id / m_Columns;
            return {
                static_cast<float>((column + 1) * m_CellWidth) / static_cast<float>(m_TextureWidth),
                static_cast<float>(m_TextureHeight - row * m_CellHeight) / static_cast<float>(m_TextureHeight)
            };
        }

        // Offset/scale that remap a mesh's full 0-1 texcoords onto the
        // sub-texture (matches SpriteRendererComponent::UVOffset/UVScale).
        glm::vec2 GetUVOffset(uint32_t id) const { return GetUVMin(id); }
        glm::vec2 GetUVScale(uint32_t id) const { return GetUVMax(id) - GetUVMin(id); }

        // Corner texcoords in quad order: bottom-left, bottom-right, top-right, top-left.
        std::array<glm::vec2, 4> GetTexCoords(uint32_t id) const
        {
            glm::vec2 min = GetUVMin(id);
            glm::vec2 max = GetUVMax(id);
            return { {
                { min.x, min.y },
                { max.x, min.y },
                { max.x, max.y },
                { min.x, max.y }
            } };
        }

    private:
        Ref<Texture2D> m_Texture; // optional; set when constructed from a texture
        uint32_t m_TextureWidth = 0, m_TextureHeight = 0;
        uint32_t m_CellWidth = 0, m_CellHeight = 0;
        uint32_t m_Columns = 0, m_Rows = 0;
    };

}
