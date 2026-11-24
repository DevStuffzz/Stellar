#include "PlayerSprintScript.h"



void PlayerSprintScript::OnCreate()
{
}

void PlayerSprintScript::OnDestroy()
{
}

void PlayerSprintScript::OnUpdate(float dt)
{
	auto& playerMovement = GetComponent<Stellar::TopDownMovementComponent>();

	playerMovement.MoveSpeed = Stellar::Input::IsKeyPressed(Stellar::KeyCode::LeftShift) ? 2.0f : 1.0f;
}
