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

	void ComponentRegistry::SwapComponent(ComponentHashCode aHashCode, void* aDataPtr1, void* aDataPtr2) const
	{
		myTypeErasureComponents.at(aHashCode).SwapFunctionPointer(aDataPtr1, aDataPtr2);
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

	ViewAndEditResult ComponentRegistry::InspectComponentProperties(size_t aHashCode, void* aData, const std::string& aVariableName) const
	{
		auto it = myTypeErasureComponents.find(aHashCode);

		if (it == myTypeErasureComponents.end())
		{
			return ViewAndEditResult{};
		}

		if (it->second.EditorFunctionPointer != nullptr)
		{
			return it->second.EditorFunctionPointer(aData, aVariableName);
		}

		ViewAndEditResult viewAndEditResult;
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

			viewAndEditResult |= ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.customVariableName);

			if (componentProperty.canEdit == false)
			{
				ImGui::EndDisabled();
			}
		}
		return viewAndEditResult;
	}

	ViewAndEditResult ComponentRegistry::ExposeProperty(size_t aHashCode, void* aData, const std::string& aVariableName) const
	{
		auto it = myTypeErasureDataTypes.find(aHashCode);

		if (it == myTypeErasureDataTypes.end())
		{
			return ViewAndEditResult{};
		}

		if (it->second.EditorFunctionPointer != nullptr)
		{
			return it->second.EditorFunctionPointer(aData, aVariableName);
		}

		ViewAndEditResult viewAndEditResult;
		for (auto& componentProperty : it->second.myComponentProperties)
		{
			viewAndEditResult |= ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
		}
		return viewAndEditResult;
	}
}