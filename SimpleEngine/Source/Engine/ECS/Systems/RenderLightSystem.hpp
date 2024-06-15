#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	class RenderLightSystem final : public ECS::System
	{
	public:
		RenderLightSystem(EntityComponentSystem& aEntityComponentSystem);
		~RenderLightSystem();

		void Init() override;
		void Update() override;
		void Render() override;
		void RenderSkyBoxAndDirectionalLight();

	private:
		EntityID mySkyBoxID;
		EntityID myDirectionalLightID;
	};
}