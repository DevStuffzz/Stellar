#pragma once

#include "Stellar.h"

class SandboxApp : public Stellar::Application
{
public:
    SandboxApp();
    ~SandboxApp() override;

protected:
    void OnUpdate(Stellar::Timestep ts) override;
    void OnWindowResize(uint32_t width, uint32_t height) override;

private:
    void CreateSceneEntities();
    void UpdateCameraProjection(uint32_t width, uint32_t height);

private:
    Stellar::OrthographicCamera m_Camera;
    float m_ZoomLevel = 1.0f;

    Stellar::Ref<Stellar::Scene> m_Scene;
    Stellar::Entity m_Player;
};
