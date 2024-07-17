#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class RenderLightSystem final : public ECS::System
	{
	public:
		explicit RenderLightSystem(EntityComponentSystem* aEntityComponentSystem);
		~RenderLightSystem();

		void Init() override;
		void Update() override;
		void Render() override;
		void LateRender() override;

		std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;
	};
}