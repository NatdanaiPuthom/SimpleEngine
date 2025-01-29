#include "Editor/Precomplied/EditorPch.hpp"
#include "TransformEntityTool.hpp"
#include "Editor/Command/Core/EditorCommandTracker.hpp"
#include "Editor/Utility/EditorUtilities.hpp"
#include "Editor/EditorProxy.hpp"

namespace Editor
{
	void TransformEntityTool::Render()
	{
		if (EditorEngine::mySelectedEntityID == GetInvalidIndex<ECS::EntityID>()) //TO-DO(v11.4.1): This shouldn't be here pls fix, future me
		{
			return;
		}

		const ImVec2 topLeft = ImGui::GetItemRectMin();
		const ImVec2 bottomRight = ImGui::GetItemRectMax();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		ImGuizmo::SetRect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

		ECS::EntityID selectedEntityID = EditorEngine::mySelectedEntityID;
		ECS::Entity& selectedEntity = MainSingleton::GetSceneManager().GetCurrentECS().GetEntity(selectedEntityID);
		ECS::TransformComponent* transformComponent = selectedEntity.GetComponent<ECS::TransformComponent>();

		if (transformComponent == nullptr)
		{
			return;
		}
		const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();

		Math::Matrix4x4f objectMatrix = transformComponent->transform.GetMatrix();
		const Math::Matrix4x4f view = camera->GetViewMatrix();
		const Math::Matrix4x4f proj = camera->GetProjectionMatrix();

		static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;

		const Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();
		if (!inputManager.GetMouseIsHidden() && !inputManager.IsKeyHeld(VK_CONTROL))
		{
			if (inputManager.IsKeyPressed('T'))
			{
				operation = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (inputManager.IsKeyPressed('R'))
			{
				operation = ImGuizmo::OPERATION::ROTATE;
			}
			else if (inputManager.IsKeyPressed('S'))
			{
				operation = ImGuizmo::OPERATION::SCALE;
			}
		}

		const float snapValue = myUseSnap ? mySnapValue : 0.f;
		const Math::Vector3f gridSnapValues(snapValue, snapValue, snapValue);

		const bool isManipulatingEntityTransform = ImGuizmo::Manipulate(&view(1, 1),
			&proj(1, 1),
			operation,
			ImGuizmo::MODE::WORLD,
			&objectMatrix(1, 1), nullptr, &gridSnapValues.x
		);

		if (isManipulatingEntityTransform && inputManager.IsKeyDown(VK_LBUTTON) && !myIsDraggingEntity)
		{
			myIsDraggingEntity = true;
			mySetEntityTransformCommand.myEntityID = selectedEntityID;
			mySetEntityTransformCommand.myOldTransform = transformComponent->transform;
		}

		if (isManipulatingEntityTransform)
		{
			switch (operation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				transformComponent->transform.SetPosition(objectMatrix.GetPosition());
				break;
			case ImGuizmo::OPERATION::ROTATE:
				transformComponent->transform.SetMatrix(objectMatrix);
				break;
			case ImGuizmo::OPERATION::SCALE:
				transformComponent->transform.SetScale(objectMatrix.GetScale());
				break;
			default:
				break;
			}
		}

		if (inputManager.IsKeyReleased(VK_LBUTTON) && myIsDraggingEntity && selectedEntityID == mySetEntityTransformCommand.myEntityID)
		{
			myIsDraggingEntity = false;
			mySetEntityTransformCommand.myNewTransform = transformComponent->transform;
			EditorProxy::GetCommandTracker().RegisterCommand(Command(mySetEntityTransformCommand, "Set Entity Transform"));
		}
	}

}