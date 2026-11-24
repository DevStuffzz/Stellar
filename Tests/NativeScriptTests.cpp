#ifdef NDEBUG
#undef NDEBUG
#endif

#include "Stellar.h"

#include <cassert>
#include <stdexcept>

namespace {
    int created = 0;
    int updated = 0;
    int destroyed = 0;

    struct TestScript : Stellar::ScriptableEntity
    {
        void OnCreate() override
        {
            ++created;
            assert(GetEntity());
        }

        void OnUpdate(float) override
        {
            ++updated;
            GetComponent<Stellar::TransformComponent>().Position.x += 1.0f;
        }

        void OnDestroy() override
        {
            assert(GetEntity());
            GetComponent<Stellar::TransformComponent>();
            ++destroyed;
        }
    };

    struct RemoveOnCreate : Stellar::ScriptableEntity
    {
        void OnCreate() override
        {
            GetEntity().RemoveComponent<Stellar::NativeScriptComponent>();
        }
        void OnUpdate(float) override { assert(false); }
        void OnDestroy() override { ++destroyed; }
    };

    struct DestroyOnUpdate : Stellar::ScriptableEntity
    {
        void OnUpdate(float) override
        {
            auto entity = GetEntity();
            entity.GetScene()->DestroyEntity(entity);
            assert(entity);
        }
        void OnDestroy() override { ++destroyed; }
    };
}

int main()
{
    {
        Stellar::Scene scene;
        auto entity = scene.CreateEntity();
        entity.AddComponent<Stellar::NativeScriptComponent>().Bind<TestScript>();
        auto camera = scene.CreateEntity();
        camera.AddComponent<Stellar::OrthographicCameraComponent>().Camera =
            Stellar::CreateRef<Stellar::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f);
        camera.AddComponent<Stellar::CameraFollowComponent>().Target = entity;

        scene.OnUpdate(0.1f);
        assert(created == 1 && updated == 1);
        assert(scene.GetPrimaryCamera().GetPosition().x == 1.0f);
        scene.OnUpdate(0.1f);
        assert(created == 1 && updated == 2);
        entity.RemoveComponent<Stellar::NativeScriptComponent>();
        assert(destroyed == 1 && !entity.HasComponent<Stellar::NativeScriptComponent>());
    }

    {
        Stellar::Scene scene;
        auto entity = scene.CreateEntity();
        entity.AddComponent<Stellar::NativeScriptComponent>().Bind<TestScript>();
        scene.OnUpdate(0.1f);
        scene.DestroyEntity(entity);
        assert(!entity && destroyed == 2);
    }

    {
        Stellar::Scene scene;
        scene.CreateEntity().AddComponent<Stellar::NativeScriptComponent>().Bind<TestScript>();
        scene.OnUpdate(0.1f);
    }
    assert(destroyed == 3);

    {
        Stellar::Scene scene;
        auto entity = scene.CreateEntity();
        entity.AddComponent<Stellar::NativeScriptComponent>().Bind<RemoveOnCreate>();
        scene.OnUpdate(0.1f);
        assert(!entity.HasComponent<Stellar::NativeScriptComponent>() && destroyed == 4);
        entity.AddComponent<Stellar::NativeScriptComponent>().Bind<DestroyOnUpdate>();
        scene.OnUpdate(0.1f);
        assert(!entity && destroyed == 5);
    }

    {
        Stellar::Scene scene;
        scene.CreateEntity().AddComponent<Stellar::NativeScriptComponent>();
        bool failed = false;
        try { scene.OnUpdate(0.1f); }
        catch (const std::logic_error&) { failed = true; }
        assert(failed);
    }
}
