#include "Engine/Precomplied/EnginePch.hpp"
#include "ScriptSystem.hpp"
#include "NodeScript/FlyExecution.hpp"
#include "NodeScript/NodeTypes/ExecutionNodes.hpp"
#include "NodeScript/FlyRegistration.hpp"
#include <imgui.h>

namespace ECS
{

	ScriptSystem::ScriptSystem()
	{
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
		}
	}

	std::unique_ptr<System> ScriptSystem::Clone() const
	{
		return std::make_unique<ScriptSystem>();
	}
}

namespace Math
{
	static Fly::eIsItemActive ViewAndEdit(Vector3f& aValue)
	{
		ImGui::DragFloat3("##", &aValue.x);

		return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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

	static Fly::eIsItemActive ViewAndEdit(Vector2f& aValue)
	{
		ImGui::DragFloat2("##", &aValue.x);

		return static_cast<Fly::eIsItemActive>(ImGui::IsItemActive());
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

namespace ECS
{
	
	using Transform = Math::Transform;
	using Vector3f = Math::Vector3f;
	using Vector2f = Math::Vector2f;

	FLY_STRUCT(Transform, Fly::eNodeOperatorTrait::None, Fly::DefaultColor, Fly::NonTargetable{});
	FLY_STRUCT(Vector3f, Fly::DefaultColor, Fly::NonTargetable{});
	FLY_STRUCT(Vector2f, Fly::DefaultColor, Fly::NonTargetable{});
	FLY_CLASS(Entity, Fly::Colors::Pink);
	FLY_CLASS(TransformComponent, Fly::DefaultColor, Fly::NonTargetable{});

	TransformComponent* GetTransformComponent(Entity* aEntity)
	{
		return aEntity->GetComponent<TransformComponent>();
	}

	std::tuple<float, float, float> BreakVector3f(const Vector3f& aVector)
	{
		return { aVector.x, aVector.y, aVector.z };
	}

	Vector3f MakeVector3f(const float aX, const float aY, const float aZ)
	{
		return Vector3f(aX, aY, aZ);
	}

	Fly::Flow ImGuiButton(const std::string& aLabel, Math::Vector2f aSize)
	{
		const std::string label = aLabel.empty() ? "Label" : aLabel;
		const bool wasClicked = ImGui::Button(label.c_str(), ImVec2{ aSize.x, aSize.y });
		return Fly::Flow(wasClicked);
	}

	struct ImGuiCheckboxState final
	{
		bool v = false;
	};

	std::tuple<Fly::Flow, Fly::Flow, Fly::Flow> ImGuiCheckbox(Fly::NodeState<ImGuiCheckboxState> aState, const std::string& aLabel)
	{
		const std::string label = aLabel.empty() ? "Label" : aLabel;
		bool wasClicked = ImGui::Checkbox(label.c_str(), &aState.mValue.v);

		return { Fly::Flow(wasClicked), Fly::Flow(aState.mValue.v), Fly::Flow(!aState.mValue.v)};
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

	FLY_FUNCTION(ImGuiButton, Fly::Directory{ "ImGui" }, Fly::InputNames{ "Label", "Size" }, Fly::OutputNames{ "On Click" }, Fly::DefaultValues{ std::string("Label") });
	FLY_FUNCTION(ImGuiCheckbox, Fly::Directory{ "ImGui" }, Fly::InputNames{ "Label" }, Fly::OutputNames{ "On Click", "On Checked", "On Unchecked" }, Fly::DefaultValues{std::string("Label")});
	FLY_FUNCTION(SetEntityPosition, Fly::MemberOf<Entity>{}, Fly::InputNames{ "Entity", "Position" });
	FLY_FUNCTION(BreakVector3f, Fly::MemberOf<Vector3f>{}, Fly::OutputNames{ "X", "Y", "Z" }, Fly::Pure{});
	FLY_FUNCTION(MakeVector3f, Fly::MemberOf<Vector3f>{}, Fly::InputNames{ "X", "Y", "Z" }, Fly::Pure{});

}
