#pragma once

#include "Stellar.h"
#include "Stellar/Scene/Tilemap.h"


    class SandboxApp : public Stellar::Application
    {
    public:
        SandboxApp();
        ~SandboxApp() override;

    protected:
        void OnUpdate(Stellar::Timestep ts) override;
        void OnWindowResize(uint32_t width, uint32_t height) override;
        void OnMouseScroll(float xOffset, float yOffset) override;

    private:
        void CreateSceneEntities();

        void UpdateTilemapMesh();

    private:
        Stellar::Ref<Stellar::Scene> m_Scene;
        Stellar::Ref<Stellar::Tilemap> m_Tilemap;
        Stellar::Ref<Stellar::TextureAtlas> m_BlocksAtlas;
        Stellar::Entity m_TilemapEntity;
        Stellar::Entity m_CameraEntity;
        Stellar::Tilemap::Bounds m_VisibleTileBounds;
        bool m_HasVisibleTileBounds = false;
        Stellar::Entity m_Player;
    };

