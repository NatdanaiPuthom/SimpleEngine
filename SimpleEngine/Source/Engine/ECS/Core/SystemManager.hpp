#pragma once
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include <memory>
#include <unordered_map>
#include <cassert>

namespace ECS
{
	class System;
	class EntityComponentSystem;
	
	class SystemManager final
	{
		using SystemHashCode = size_t;

		friend class ECS::EntityComponentSystem;
	public:

		void Init();
		void Update();
		void UpdateRenderSystem();
		void Render();
		void RenderPointLights();
		void RenderSkyBoxAndDirectionalLight();

		template<typename T>
		void AddSystem(EntityComponentSystem* aEntityComponentSystem);

	private:
		void EarlyUpdate();
		void FixedUpdate();
		void LateUpdate();
		void NormalUpdate();

	private:
		explicit SystemManager(EntityComponentSystem* aEntityComponentSystem);
		~SystemManager();
	private:

		std::unordered_map<SystemHashCode, std::shared_ptr<System>> mySystems;
		std::shared_ptr<RenderLightSystem> mySkyBoxAndDirectionalLightSystem;
		std::shared_ptr<RenderSystem> myRenderSystem;

		float myFixedUpdateTime;
		float myTimer;
	};

	template<typename T>
	inline void SystemManager::AddSystem(EntityComponentSystem* aEntityComponentSystem)
	{
		const SystemHashCode hashCode = typeid(T).hash_code();

		if (mySystems.contains(hashCode))
		{
			assert(false && "System already exist.");
			return;
		}

		mySystems[hashCode] = std::make_shared<T>(aEntityComponentSystem);
	}
}