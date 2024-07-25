#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class CameraSystem : public ECS::System
	{
	public:
		CameraSystem();
		~CameraSystem() override;

		void Update(EntityComponentSystem* aEntityComponentSystem) override;

		std::unique_ptr<System> Clone() const override;
	};
}