#include "Stellar/Scene/Scene.h"

#include "Stellar/Scene/Components.h"
#include "Stellar/Scene/Entity.h"
#include "Stellar/Renderer/Renderer.h"

namespace Stellar {

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity(m_Registry.Create(), this);
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.Destroy(entity.GetID());
    }

    void Scene::OnRender(const OrthographicCamera& camera)
    {
        // Created lazily so a Scene can be constructed before the renderer.
        if (!m_QuadMesh)
            m_QuadMesh = Mesh::CreateQuad();

        Renderer::BeginScene(camera);

        m_Registry.Each<TransformComponent, SpriteRendererComponent>(
            [this](EntityID, TransformComponent& transform, SpriteRendererComponent& sprite)
            {
                const Ref<Mesh>& mesh = sprite.Mesh ? sprite.Mesh : m_QuadMesh;
                if (sprite.Texture)
                    Renderer::Submit(mesh, sprite.Texture, transform.GetTransform(), sprite.Color, sprite.UVOffset, sprite.UVScale);
                else
                    Renderer::Submit(mesh, transform.GetTransform(), sprite.Color);
            });

        Renderer::EndScene();
    }

}
