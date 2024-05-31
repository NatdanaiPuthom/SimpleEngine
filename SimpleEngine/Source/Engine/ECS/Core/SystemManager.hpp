#pragma once
#include "Engine/ECS/Core/System.hpp"
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include <memory>
#include <vector>

namespace ECS
{
	class SystemManager final
	{
	public:
		SystemManager(EntityManager* aEntityManager);
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

		EntityManager* myEntityManager;

		const float myFixedUpdateTime;
		float myTimer;
	};
}