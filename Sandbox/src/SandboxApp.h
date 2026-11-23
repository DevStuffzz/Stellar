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
    void UpdateCameraProjection(uint32_t width, uint32_t height);

private:
    Stellar::OrthographicCamera m_Camera;
    float m_ZoomLevel = 1.0f;

    Stellar::Ref<Stellar::Mesh> m_Quad;
    Stellar::Ref<Stellar::Mesh> m_Triangle;
    Stellar::Ref<Stellar::Mesh> m_Circle;

    glm::vec2 m_PlayerPosition = { 0.0f, 0.0f };
    float m_PlayerRotation = 0.0f;
};
