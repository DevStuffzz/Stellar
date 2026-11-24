#pragma once

#include "Core.h"
#include "Stellar/Renderer/TextureAtlas.h"

#include <cstdint>
#include <vector>

namespace Stellar {

    // A flipbook animation over a TextureAtlas, defined by sub-texture ids.
    struct SpriteAnimation
    {
        Ref<TextureAtlas> Atlas;
        std::vector<uint32_t> Frames;
        float FrameTime = 0.1f; // seconds per frame
        bool Loop = true;

        SpriteAnimation() = default;

        // Frames from the inclusive id range [firstID, lastID].
        SpriteAnimation(const Ref<TextureAtlas>& atlas, uint32_t firstID, uint32_t lastID, float frameTime = 0.1f, bool loop = true)
            : Atlas(atlas), FrameTime(frameTime), Loop(loop)
        {
            ST_ASSERT(firstID <= lastID, "Animation id range is reversed!");
            ST_ASSERT(atlas && lastID < atlas->GetSubTextureCount(), "Animation id range is outside the atlas!");
            Frames.reserve(lastID - firstID + 1);
            for (uint32_t id = firstID; id <= lastID; id++)
                Frames.push_back(id);
        }

        // Explicit frame id list (for non-contiguous or ping-pong sequences).
        SpriteAnimation(const Ref<TextureAtlas>& atlas, std::vector<uint32_t> frames, float frameTime = 0.1f, bool loop = true)
            : Atlas(atlas), Frames(std::move(frames)), FrameTime(frameTime), Loop(loop)
        {
        }

        bool IsValid() const { return Atlas && !Frames.empty() && FrameTime > 0.0f; }
        float GetDuration() const { return Frames.size() * FrameTime; }
    };

}
