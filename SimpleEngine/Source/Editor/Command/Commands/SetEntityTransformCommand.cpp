#include "Editor/Precomplied/EditorPch.hpp"
#include "SetEntityTransformCommand.hpp"

namespace Editor
{

	void SetEntityTransformCommand::Execute() const
	{
		SetTransform(myNewTransform);
	}

	void SetEntityTransformCommand::Undo() const
	{
		SetTransform(myOldTransform);
	}

	void SetEntityTransformCommand::SetTransform(Math::Transform aTransform) const
	{
		ECS::Entity& selectedEntity = MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(myEntityID);
		ECS::TransformComponent* transformComponent = selectedEntity.GetComponent<ECS::TransformComponent>();

		transformComponent->transform = aTransform;
	}

}