#pragma once
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include <memory>
#include <vector>

namespace ECS
{
	class System;

	class SystemManager final
	{
	public:
		SystemManager(EntityComponentSystem& aEntityComponentSystem);
		~SystemManager();

		void Init();
		void Update();
		void Render();
		void RenderPointLights();
		void RenderSkyBoxAndDirectionalLight();

		void AddSystem(std::unique_ptr<System> aSystem);

	private:
		std::vector<std::unique_ptr<System>> mySystems;
		RenderLightSystem mySkyBoxAndDirectionalLightSystem;

		const float myFixedUpdateTime;
		float myTimer;
	};
}