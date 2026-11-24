#pragma once

#include "Stellar.h"


class PlayerSprintScript : public Stellar::ScriptableEntity
{
	public:
		PlayerSprintScript() = default;
		virtual ~PlayerSprintScript() = default;

		void OnCreate() override;
		void OnDestroy() override;
		void OnUpdate(float dt) override;
};
