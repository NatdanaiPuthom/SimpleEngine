#include "Engine/Precomplied/EnginePch.hpp"
#include "ScriptSystem.hpp"
#include "NodeScript/FlyExecution.hpp"
#include "NodeScript/NodeTypes/ExecutionNodes.hpp"
#include "NodeScript/FlyRegistration.hpp"
#include "NodeScript/Fly.hpp"
#include <imgui.h>
#include "SimpleUtilities/Color.hpp"

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

namespace SimpleUtilities
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
		aValue = DataTypeFacade(Global::GetDataTypeManager().GetDataTypeIDByName(name));
	}
}

namespace ECS
{

	using Transform = Math::Transform;
	using Vector3f = Math::Vector3f;
	using Vector2f = Math::Vector2f;
	using Color = SimpleUtilities::Color;
	using DataType = Fly::DataTypeFacade;

	FLY_VALUETYPE(Transform, Fly::eNodeOperatorTrait::None, Fly::NonTargetable{});
	FLY_VALUETYPE(Vector3f, Fly::NonTargetable{});

	FLY_MEMBER(Vector3f::x);
	FLY_MEMBER(Vector3f::y);
	FLY_MEMBER(Vector3f::z);
	FLY_VALUETYPE(Vector2f, Fly::NonTargetable{});
	FLY_POINTERTYPE(Entity, Fly::Colors::Pink);
	FLY_POINTERTYPE(TransformComponent, Fly::NonTargetable{});
	FLY_VALUETYPE(Color);
	FLY_VALUETYPE(DataType);

	TransformComponent* GetTransformComponent(Entity* aEntity)
	{
		return aEntity->GetComponent<TransformComponent>();
	}

	std::tuple<float, float, float> BreakVector3f(const Vector3f& aVector)
	{
		return { aVector.x, aVector.y, aVector.z };
	}

	Transform MakeTransfrom(const Vector3f& aPosition, const Vector3f& aRotation, const Vector3f& aScale)
	{
		Transform t;
		t.SetPosition(aPosition);
		t.SetRotation(aRotation);
		t.SetScale(aScale);

		return t;
	}

	std::tuple<Vector3f, Vector3f, Vector3f> BreakTransform(const Transform& aTransform)
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

	Vector3f MakeVector3f(const float aX, const float aY, const float aZ)
	{
		return Vector3f(aX, aY, aZ);
	}

	void SetEntityPosition(Entity* aEntity, const Vector3f& aPosition)
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

	void SetDataTypeColor(Fly::DataTypeFacade aFacade, SimpleUtilities::Color aColor)
	{
		if (aFacade)
		{
			aFacade.SetColor(Fly::Color(aColor.r, aColor.g, aColor.b, aColor.a));
		}
	}

	FLY_FUNCTION(SetEntityPosition, Fly::MemberOf<Entity>{}, Fly::InputNames{ "Entity", "Position" });
	FLY_FUNCTION(BreakVector3f, Fly::MemberOf<Vector3f>{}, Fly::OutputNames{ "X", "Y", "Z" }, Fly::Pure{});
	FLY_FUNCTION(MakeVector3f, Fly::MemberOf<Vector3f>{}, Fly::InputNames{ "X", "Y", "Z" }, Fly::Pure{});
	FLY_FUNCTION(MakeTransfrom, Fly::MemberOf<Transform>{}, Fly::InputNames{ "Position", "Rotation", "Scale" }, Fly::Pure{});
	FLY_FUNCTION(BreakTransform, Fly::MemberOf<Transform>{}, Fly::OutputNames{ "Position", "Rotation", "Scale" }, Fly::Pure{});
	FLY_FUNCTION(GetStringLength, Fly::Pure{});
	FLY_FUNCTION(ClearString);
	FLY_FUNCTION(MakeString, Fly::Pure{});
	FLY_FUNCTION(ToggleBool);
	FLY_FUNCTION(OnDoorOpen, Fly::Event{});
	FLY_FUNCTION(SetDataTypeColor)
}
