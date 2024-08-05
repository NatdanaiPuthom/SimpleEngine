#include "Engine/Precomplied/EnginePch.hpp"
#include "ScriptSystem.hpp"
#include "NodeScript/SimpleScript/Core/FlyExecution.hpp"
#include "NodeScript/SimpleScript/Core/NodeTypes/ExecutionNodes.hpp"
#include "NodeScript/SimpleScript/Core/FlyRegistration.hpp"
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

			scriptComponent->classInstanceView.Init();
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

			scriptComponent->classInstanceView.ExecuteEvent(Fly::Tick, &entity, executionContext);
		}

		//Fly::RegisterFunctionNode r = Fly::RegisterFunctionNode::Register(&Entity::GetComponent<TransformComponent>, "");
	}

	std::unique_ptr<System> ScriptSystem::Clone() const
	{
		return std::make_unique<ScriptSystem>();
	}
}

namespace Math
{
	static bool Edit(Vector3f& aValue)
	{
		return ImGui::DragFloat3("##", &aValue.x);
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

	static bool Edit(Vector2f& aValue)
	{
		return ImGui::DragFloat2("##", &aValue.x);
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

	FLY_STRUCT(Transform, Fly::eNodeOperatorTrait::None, Fly::DefaultColor, Fly::NonTargetable);
	FLY_STRUCT(Vector3f, Fly::eNodeOperatorTrait::All, Fly::DefaultColor, Fly::NonTargetable);
	FLY_STRUCT(Vector2f, Fly::eNodeOperatorTrait::All, Fly::DefaultColor, Fly::NonTargetable);
	FLY_CLASS(Entity, Fly::eNodeOperatorTrait::None, Fly::Colors::Pink);
	FLY_CLASS(TransformComponent, Fly::eNodeOperatorTrait::None, Fly::DefaultColor, Fly::NonTargetable);

	FLY_MEMBER(TransformComponent::transform);

	FLY_FUNCTION(Entity::GetName, "Entity");
	FLY_FUNCTION(Entity::SetName, "Entity");
	//FLY_FUNCTION(Entity::GetComponent<TransformComponent>, "Entity");

	FLY_FUNCTION(Transform::GetPosition, "Transform");
	FLY_FUNCTION(Transform::SetPosition, "Transform");

	TransformComponent* GetTransformComponent(Entity* aEntity)
	{
		return aEntity->GetComponent<TransformComponent>();
	}

	std::tuple<float, float, float> BreakVector3f(const Vector3f& aVector)
	{
		return { aVector.x, aVector.y, aVector.z };
	}

	bool ImGuiButton(const std::string& aLabel, Math::Vector2f aSize)
	{
		const std::string label = aLabel.empty() ? "Label" : aLabel;
		return ImGui::Button(label.c_str(), ImVec2{ aSize.x, aSize.y });
	}

	FLY_FUNCTION(ImGuiButton, "ImGui", Fly::InputNames{ "Label", "Size"}, Fly::OutputNames{ "Was Clicked" }, Fly::DefaultValues{ std::string("Label") });

	FLY_FUNCTION(BreakVector3f, "Vector3f", Fly::OutputNames{ "X", "Y", "Z" }, Fly::Pure{});
	FLY_FUNCTION(GetTransformComponent, "Entity", Fly::Pure{});

}
