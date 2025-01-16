#include "Engine/Precomplied/EnginePch.hpp"
#include "ScriptSystem.hpp"
#include "NodeScript/FlyExecution.hpp"
#include "NodeScript/NodeTypes/ExecutionNodes.hpp"
#include "NodeScript/FlyRegistration.hpp"
#include "NodeScript/Fly.hpp"
#include <imgui.h>
#include "SimpleUtilities/Color.hpp"
#include "SimpleScript/NodeScriptCustomTypes.hpp"

namespace ECS
{

	ScriptSystem::ScriptSystem()
	{
	}

	std::tuple<Fly::Flow, float> OnDoorOpen()
	{
		return { Fly::Flow{ true }, 5.5f };
	}


	void ScriptSystem::Init(EntityComponentSystem* aEntityComponentSystem)
	{
		auto& entityIDs = aEntityComponentSystem->GetEntityIDsWithThisComponent<ScriptComponent>();

		for (auto& entityID : entityIDs)
		{
			ECS::Entity& entity = aEntityComponentSystem->GetEntity(entityID);

			ScriptComponent* const scriptComponent = entity.GetComponent<ScriptComponent>();

			scriptComponent->classInstanceFacade.InitRuntime();
		}
	}

	void ScriptSystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		auto& entityIDs = aEntityComponentSystem->GetEntityIDsWithThisComponent<ScriptComponent>();

		for (auto& entityID : entityIDs)
		{
			ECS::Entity& entity = aEntityComponentSystem->GetEntity(entityID);

			ScriptComponent* const scriptComponent = entity.GetComponent<ScriptComponent>();

			const Fly::ExecutionContextBase executionContext
			{
				.mDeltaTime = Global::GetDeltaTime()
			};

			scriptComponent->classInstanceFacade.ExecuteEvent(Fly::Tick, &entity, executionContext);

			if (MainSingleton::GetInputManager().IsKeyPressed('H'))
			{
				scriptComponent->classInstanceFacade.ExecuteEvent(OnDoorOpen, &entity, Fly::ExecutionContextBase{});
			}
		}
	}

	std::unique_ptr<System> ScriptSystem::Clone() const
	{
		return std::make_unique<ScriptSystem>();
	}
}

namespace Math
{
	static Fly::ViewAndEditResult ViewAndEdit(Vector3f& aValue)
	{
		ImGui::DragFloat3("##", &aValue.x);

		Fly::ViewAndEditResult result;
		result.mIsItemActive = ImGui::IsItemActive();
		return result;
	}

	static void Save(const Vector3f& aValue, nlohmann::json& aJson)
	{
		aJson["x"] = aValue.x;
		aJson["y"] = aValue.y;
		aJson["z"] = aValue.z;
	}

	static void Load(Vector3f& aValue, const nlohmann::json& aJson)
	{
		aValue.x = aJson["x"];
		aValue.y = aJson["y"];
		aValue.z = aJson["z"];
	}

	static Fly::ViewAndEditResult ViewAndEdit(Vector2f& aValue)
	{
		ImGui::DragFloat2("##", &aValue.x);

		Fly::ViewAndEditResult result;
		result.mIsItemActive = ImGui::IsItemActive();
		return result;
	}

	static void Save(const Vector2f& aValue, nlohmann::json& aJson)
	{
		aJson["x"] = aValue.x;
		aJson["y"] = aValue.y;
	}

	static void Load(Vector2f& aValue, const nlohmann::json& aJson)
	{
		aValue.x = aJson["x"];
		aValue.y = aJson["y"];
	}
}

namespace Simple
{

	static Fly::ViewAndEditResult ViewAndEdit(Color& aValue)
	{
		ImGui::ColorEdit4("##", &aValue.r);

		Fly::ViewAndEditResult result;
		result.mIsItemActive = ImGui::IsItemActive();
		return result;
	}

	static void Save(const Color& aValue, nlohmann::json& aJson)
	{
		aJson["x"] = aValue.r;
		aJson["g"] = aValue.g;
		aJson["b"] = aValue.b;
		aJson["a"] = aValue.a;
	}

	static void Load(Color& aValue, const nlohmann::json& aJson)
	{
		aValue.r = aJson["r"];
		aValue.g = aJson["g"];
		aValue.b = aJson["b"];
		aValue.a = aJson["a"];
	}
}

namespace Fly
{

	static Fly::ViewAndEditResult ViewAndEdit(DataTypeFacade& aValue)
	{
		Fly::ViewAndEditResult result;
		if (ImGui::BeginCombo("##", aValue.GetName().c_str()))
		{

			result.mIsItemActive = ImGui::IsItemActive();

			const std::vector<Fly::DataTypeFacade> dataTypeFacades = Fly::GetDataTypes();

			for (const DataTypeFacade& dataTypeFacade : dataTypeFacades)
			{
				if (ImGui::Selectable(dataTypeFacade.GetName().c_str()))
				{
					aValue = dataTypeFacade;
				}
			}
			ImGui::EndCombo();
		}

		return result;
	}

	static void Save(const DataTypeFacade& aValue, nlohmann::json& aJson)
	{
		aJson["Name"] = aValue.GetName();
	}

	static void Load(DataTypeFacade& aValue, const nlohmann::json& aJson)
	{
		const std::string& name = aJson["Name"];
		aValue = Fly::GetDataTypeFacadeByName(name);
	}
}

namespace ECS
{

	FLY_VALUETYPE(Math::Transform, Fly::CustomName{ "Transform" }, Fly::eNodeOperatorTrait::None, Fly::NonTargetable{});
	FLY_VALUETYPE(Math::Vector3f, Fly::CustomName{ "Vector3f" }, Fly::NonTargetable{});
	FLY_MEMBER(Math::Vector3f::x);
	FLY_MEMBER(Math::Vector3f::y);
	FLY_MEMBER(Math::Vector3f::z);
	FLY_VALUETYPE(Math::Vector2f, Fly::CustomName{ "Vector2f" }, Fly::NonTargetable{});
	FLY_POINTERTYPE(Entity);
	FLY_POINTERTYPE(TransformComponent, Fly::NonTargetable{});
	FLY_VALUETYPE(Simple::Color, Fly::CustomName{ "Color" });
	FLY_VALUETYPE(Fly::DataTypeFacade, Fly::CustomName{ "FlyDataType" });

	TransformComponent* GetTransformComponent(Entity* aEntity)
	{
		return aEntity->GetComponent<TransformComponent>();
	}

	std::tuple<float, float, float> BreakVector3f(const Math::Vector3f& aVector)
	{
		return { aVector.x, aVector.y, aVector.z };
	}

	Math::Vector3f MakeVector3f(const float aX, const float aY, const float aZ)
	{
		return Math::Vector3f(aX, aY, aZ);
	}

	Math::Transform MakeTransfrom(const Math::Vector3f& aPosition, const Math::Vector3f& aRotation, const Math::Vector3f& aScale)
	{
		Math::Transform t;
		t.SetPosition(aPosition);
		t.SetRotation(aRotation);
		t.SetScale(aScale);

		return t;
	}

	std::tuple<Math::Vector3f, Math::Vector3f, Math::Vector3f> BreakTransform(const Math::Transform& aTransform)
	{
		return std::tuple{ aTransform.GetPosition(), aTransform.GetRotation(), aTransform.GetScale() };
	}

	size_t GetStringLength(const std::string& aString)
	{
		return aString.size();
	}

	void ClearString(std::string* aString)
	{
		aString->clear();
	}

	std::string MakeString(std::string aString)
	{
		return aString;
	}

	void SetEntityPosition(Entity* aEntity, const Math::Vector3f& aPosition)
	{
		if (!aEntity)
		{
			return;
		}

		TransformComponent* transformComponent = aEntity->GetComponent<TransformComponent>();

		if (!transformComponent)
		{
			return;
		}
		transformComponent->transform.SetPosition(aPosition);
	}

	void ToggleBool(bool* aBool)
	{
		if (!aBool)
		{
			return;
		}
		*aBool = !*aBool;
	}

	void SetDataTypeColor(Fly::DataTypeFacade aFacade, Simple::Color aColor)
	{
		if (aFacade)
		{
			aFacade.SetColor(Fly::Color(aColor.r, aColor.g, aColor.b, aColor.a));
		}
	}

	FLY_FUNCTION(SetEntityPosition, Fly::MemberOf<Entity>{}, Fly::InputNames{ "Entity", "Position" });
	FLY_FUNCTION(BreakVector3f, Fly::MemberOf<Math::Vector3f>{}, Fly::OutputNames{ "X", "Y", "Z" }, Fly::Pure{});
	FLY_FUNCTION(MakeVector3f, Fly::MemberOf<Math::Vector3f>{}, Fly::InputNames{ "X", "Y", "Z" }, Fly::Pure{});
	FLY_FUNCTION(MakeTransfrom, Fly::MemberOf<Math::Transform>{}, Fly::InputNames{ "Position", "Rotation", "Scale" }, Fly::Pure{});
	FLY_FUNCTION(BreakTransform, Fly::MemberOf<Math::Transform>{}, Fly::OutputNames{ "Position", "Rotation", "Scale" }, Fly::Pure{});
	FLY_FUNCTION(GetStringLength, Fly::Pure{});
	FLY_FUNCTION(ClearString);
	FLY_FUNCTION(MakeString, Fly::Pure{});
	FLY_FUNCTION(ToggleBool);
	FLY_FUNCTION(OnDoorOpen, Fly::Event{});
	FLY_FUNCTION(SetDataTypeColor)
}
