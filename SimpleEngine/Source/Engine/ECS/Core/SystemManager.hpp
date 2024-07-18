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
		SystemManager(const SystemManager& aOther);
		SystemManager& operator=(const SystemManager& aOther);

		void Init();
		void Update(EntityComponentSystem* aEntityComponentSystem);
		void Render(EntityComponentSystem* aEntityComponentSystem);

		template<typename T>
		void AddSystem();

	private:
		void EarlyUpdate(EntityComponentSystem* aEntityComponentSystem);
		void FixedUpdate(EntityComponentSystem* aEntityComponentSystem);
		void LateUpdate(EntityComponentSystem* aEntityComponentSystem);
		void LateRender(EntityComponentSystem* aEntityComponentSystem);
	private:
		 SystemManager();
		~SystemManager();
	private:
		std::unordered_map<SystemHashCode, std::unique_ptr<System>> mySystems;
		char myPadding[48] = "Never give up on your dreams! Gotta Believe!!!\0";
	};

	template<typename T>
	inline void SystemManager::AddSystem()
	{
		const SystemHashCode hashCode = typeid(T).hash_code();

		if (mySystems.contains(hashCode))
		{
			assert(false && "System already exist.");
			return;
		}

		mySystems.emplace(hashCode, std::make_unique<T>());
	}
}