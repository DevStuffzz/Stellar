#pragma once

#include "Core.h"
#include "Stellar/Scene/Registry.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/OrthographicCamera.h"
#include "Stellar/Timestep.h"

#include <deque>
#include <string>

namespace Stellar {

    class Entity;
    struct OrthographicCameraComponent;

    class ST_API Scene
    {
    public:
        Scene() = default;
        ~Scene();
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Creates an entity with a TagComponent and a TransformComponent.
        Entity CreateEntity(const std::string& name = "Entity");
        void DestroyEntity(Entity entity);
        void RemoveNativeScript(EntityID entity);

        // Advances scene systems (e.g. sprite animators).
        void OnUpdate(Timestep ts);

        void OnViewportResize(uint32_t width, uint32_t height);
        void OnMouseScroll(float xOffset, float yOffset);

        const OrthographicCamera& GetPrimaryCamera() const;

        // Draws tilemaps and sprites in entity creation order.
        void OnRender();

        Registry& GetRegistry() { return m_Registry; }

    private:
        void UpdateCameraProjection(OrthographicCameraComponent& camera);
        void ProcessScriptOperations();

        struct ScriptOperation
        {
            EntityID Entity;
            bool DestroyEntity;
        };

        Registry m_Registry;
        Ref<Mesh> m_QuadMesh; // lazily created default sprite mesh
        std::deque<ScriptOperation> m_ScriptOperations;
        unsigned int m_ScriptCallbackDepth = 0;
        bool m_ProcessingScriptOperations = false;

        std::shared_ptr<OrthographicCamera> m_MainCamera;
        uint32_t m_ViewportWidth = 1;
        uint32_t m_ViewportHeight = 1;
    };

}
