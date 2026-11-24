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

    private:
        Stellar::Ref<Stellar::Scene> m_Scene;
        Stellar::Entity m_CameraEntity;
        Stellar::Entity m_Player;
    };

