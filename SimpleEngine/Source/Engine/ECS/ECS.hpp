#pragma once
#include "Engine/ECS/Components/AllEngineComponents.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"
#include "Engine/ECS/Core/ComponentManager.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include <memory>
#include <unordered_set>

namespace ECS
{
	class System;
}

namespace ECS
{
	class EntityComponentSystem final
	{
	public:
		EntityComponentSystem();
		~EntityComponentSystem();

		EntityComponentSystem(const EntityComponentSystem& aOther);
		EntityComponentSystem(EntityComponentSystem&& aOther) noexcept;
		EntityComponentSystem& operator=(const EntityComponentSystem& aOther);
		EntityComponentSystem& operator=(EntityComponentSystem&& aOther) noexcept;

		static void SaveData(EntityComponentSystem& aECS, const std::string& aFileName);
		static void LoadData(EntityComponentSystem& aECS, const std::string& aFileName);

		void Init();
		void Update();
		void Render();

		void EarlyUpdate();
		void FixedUpdate();
		void LateUpdate();
		void LateRender();
		void RenderEntitiesWithBloomComponent();

		void AddClonedSystem(const size_t aSystemHashCode, std::unique_ptr<System> aSystem);

		ECS::Entity& CreateEntity(const EntityID aEntityID = 0);
		bool RemoveEntity(const EntityID aEntityID);
	public:
		ECS::Entity& GetEntity(const EntityID aID);
		std::vector<ECS::Entity>& GetAllEntities();
		void* GetComponentPointerByComponentID(const ComponentID aComponentID);

		template<typename T>
		const std::unordered_set<EntityID>& GetEntityIDsWithThisComponent();
	private:
		static void EraseMissingElementFromJSON(const nlohmann::json& aJsonData, const std::string& aAbsolutePath, const size_t aEntityIndex, const size_t aComponentIndex);
		static void LoadComponentData(nlohmann::json& aPropertiesJSON, const std::vector<ComponentProperty>& aComponentProperties,const ComponentRegistry* aComponentRegistry, void* aComponentPointer);
	private:
		EntityManager myEntityManager;
		ComponentManager myComponentManager;
		SystemManager mySystemManager;
	};

	template<typename T>
	inline const std::unordered_set<EntityID>& EntityComponentSystem::GetEntityIDsWithThisComponent()
	{
		return myComponentManager.GetEntityIDsWithThisComponent<T>();
	}
}