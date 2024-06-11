#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/ComponentManager.hpp"
#include "Engine/ECS/Components/Core/NullComponent.hpp"
#include "Engine/ECS/ECSReflection.hpp"

namespace ECS
{
	ComponentManager::ComponentManager()
		: myCurrentComponentID(0)
	{
	}

	ComponentManager::~ComponentManager()
	{
		for (auto& [componentType, componentPool] : myComponents)
		{
			const size_t componentCount = componentPool.GetComponentCount();

			if (componentCount == 0)
			{
				continue;
			}

			const size_t sizeOfComponentType = componentPool.GetOccupiedMemorySpace() / componentCount;
			char* component = componentPool.GetStartMemoryAddress();

			for (size_t i = 0; i < componentCount; ++i)
			{
				ComponentRegistry::myTypeErasureComponentDestructorInvoker[componentType](&component[i * sizeOfComponentType]);;
			}
		}
	}

	bool ComponentManager::RemoveComponentByTypeIndex(const ComponentType& aComponentType, const size_t aComponentID)
	{
		ComponentPool& pool = myComponents[aComponentType];
		char* component = pool.GetComponentAddressByID(aComponentID);

		ComponentRegistry::myTypeErasureComponentDestructorInvoker[aComponentType](static_cast<void*>(component));
		myAllComponents.erase(aComponentID);

		return pool.SwapWithLastAndRemoveEditor(aComponentID);
	}
}