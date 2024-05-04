#pragma once
#include "Components/Base/Component.h"

class PlayerComponent final : public Component
{
public:
	PlayerComponent() 
	{
	}

	~PlayerComponent() override {};

	void Update(const GameUpdateContext& aContext) override;

	const int myID = 1;
};