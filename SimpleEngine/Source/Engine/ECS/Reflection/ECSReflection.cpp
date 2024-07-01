#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	ComponentRegistry::ComponentRegistry()
	{
	}

	ComponentRegistry::~ComponentRegistry()
	{
	}

	ComponentRegistry* ComponentRegistry::GetInstance()
	{
		if (myPtr == nullptr)
		{
			myPtr = new ComponentRegistry();
		}

		return myPtr;
	}

	void ComponentRegistry::Destroy()
	{
		if (myPtr != nullptr)
		{
			delete myPtr;
			myPtr = nullptr;
		}
	}

	void ComponentRegistry::InspectComponentProperties(size_t aHashCode, void* aData, const std::string& aVariableName)
	{
		auto it = myTypeErasureComponents.find(aHashCode);

		if (it != myTypeErasureComponents.end())
		{
			if (it->second.EditorFunctionPointer != nullptr)
			{
				it->second.EditorFunctionPointer(aData, aVariableName);
			}

			for (auto& componentProperty : it->second.myComponentProperties)
			{
				if (componentProperty.shouldExpose == false)
				{
					continue;
				}

				ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
			}
		}
	}

	void ComponentRegistry::ExposeProperty(size_t aHashCode, void* aData, const std::string& aVariableName)
	{
		auto it = myTypeErasureDataTypes.find(aHashCode);

		if (it != myTypeErasureDataTypes.end())
		{
			if (it->second.EditorFunctionPointer != nullptr)
			{
				it->second.EditorFunctionPointer(aData, aVariableName);
			}

			for (auto& componentProperty : it->second.myComponentProperties)
			{
				ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
			}
		}
	}
}