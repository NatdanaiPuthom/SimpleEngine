#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "External/imgui.h"

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


	ComponentID ComponentRegistry::AddComponent(ComponentHashCode aHashCode, Entity& aEntity) const
	{
		return myTypeErasureComponents.at(aHashCode).AddComponentFunctionPointer(aEntity);
	}


	void ComponentRegistry::CopyComponent(ComponentHashCode aHashCode, void* aDestination, const void* aSource) const
	{
		myTypeErasureComponents.at(aHashCode).CopyFunctionPointer(aDestination, aSource);
	}

	size_t ComponentRegistry::GetComponentSize(ComponentHashCode aHashCode) const
	{
		return myTypeErasureComponents.at(aHashCode).mySize;
	}

	InplaceAllocateFunction ComponentRegistry::GetInplaceAllocateFunction(ComponentHashCode aHashCode) const
	{
		return myTypeErasureComponents.at(aHashCode).InplaceAllocate;
	}

	DestroyFunction ComponentRegistry::GetDestroyFunction(ComponentHashCode aHashCode) const
	{
		return myTypeErasureComponents.at(aHashCode).Destroy;
	}

	CopyFunction ComponentRegistry::GetCopyFunction(ComponentHashCode aHashCode) const
	{
		return myTypeErasureComponents.at(aHashCode).CopyFunctionPointer;
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

				if (componentProperty.canEdit == false)
				{
					ImGui::BeginDisabled();
				}

				ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);

				if (componentProperty.canEdit == false)
				{
					ImGui::EndDisabled();
				}
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