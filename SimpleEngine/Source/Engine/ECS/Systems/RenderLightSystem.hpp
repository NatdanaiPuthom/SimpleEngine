#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class RenderLightSystem final : public ECS::System
	{
	public:
		RenderLightSystem();
		~RenderLightSystem();

		void Render(EntityComponentSystem* aEntityComponentSystem) override;
		void LateRender(EntityComponentSystem* aEntityComponentSystem) override;

		std::unique_ptr<System> Clone() const override;
	};
}