#pragma once

#include "Core.h"
#include "Stellar/Scene/Registry.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/OrthographicCamera.h"

#include <string>

namespace Stellar {

    class Entity;

    class ST_API Scene
    {
    public:
        Scene() = default;
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Creates an entity with a TagComponent and a TransformComponent.
        Entity CreateEntity(const std::string& name = "Entity");
        void DestroyEntity(Entity entity);

        // Draws every entity with Transform + SpriteRenderer in creation order.
        void OnRender(const OrthographicCamera& camera);

        Registry& GetRegistry() { return m_Registry; }

    private:
        Registry m_Registry;
        Ref<Mesh> m_QuadMesh; // lazily created default sprite mesh
    };

}
