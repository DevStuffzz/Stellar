#pragma once

#include "Core.h"
#include "Stellar/Scene/Scene.h"

#include <utility>

namespace Stellar {

    // Lightweight handle into a Scene's registry; cheap to copy.
    class Entity
    {
    public:
        Entity() = default;
        Entity(EntityID id, Scene* scene)
            : m_ID(id), m_Scene(scene) {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            ST_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->GetRegistry().Emplace<T>(m_ID, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            return m_Scene->GetRegistry().Get<T>(m_ID);
        }

        template<typename T>
        bool HasComponent() const
        {
            return m_Scene && m_Scene->GetRegistry().Has<T>(m_ID);
        }

        template<typename T>
        void RemoveComponent()
        {
            m_Scene->GetRegistry().Remove<T>(m_ID);
        }

        EntityID GetID() const { return m_ID; }
        Scene* GetScene() const { return m_Scene; }

        operator bool() const { return m_ID != NullEntity && m_Scene && m_Scene->GetRegistry().Valid(m_ID); }
        bool operator==(const Entity& other) const { return m_ID == other.m_ID && m_Scene == other.m_Scene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

    private:
        EntityID m_ID = NullEntity;
        Scene* m_Scene = nullptr;
    };

}
