#pragma once
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
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
		void Render();
		void RenderPointLights();
		void RenderSkyBoxAndDirectionalLight();

		template<typename T>
		void AddSystem(EntityComponentSystem* aEntityComponentSystem);

	private:
		explicit SystemManager(EntityComponentSystem* aEntityComponentSystem);
		~SystemManager();

		std::unordered_map<SystemHashCode, std::shared_ptr<System>> mySystems;
		std::shared_ptr<RenderLightSystem> mySkyBoxAndDirectionalLightSystem;

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