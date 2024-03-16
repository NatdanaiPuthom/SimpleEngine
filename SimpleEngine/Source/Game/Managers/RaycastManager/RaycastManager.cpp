#include "Game/Precomplied/GamePch.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"
#include "Game/Navmesh/Navmesh.hpp"

namespace Simple
{
	RaycastManager::RaycastManager()
		: myRaycastLine(std::make_unique<Drawer::Line>())
		, myDebugSphere(std::make_unique<Drawer::Sphere>())
		, mySelectedModelIndex(-1)
		, mySelectDelay(0.1f)
		, myTimer(0.0f)
		, myShouldRenderDebugSphere(false)
	{
		myRaycastLine->color = { 1,0,0, 1 };
		myRaycastLine->startPosition = { 0,0,0 };
		myRaycastLine->endPosition = { 0,0,0 };

		myDebugSphere->radius = 0.2f;
		myDebugSphere->color = { 1.0f, 0.0f, 0.0f, 0.0f };

	}

	RaycastManager::~RaycastManager()
	{
	}

	void RaycastManager::Init()
	{
	}

	void RaycastManager::Update()
	{
		if (SimpleUtilities::InputManager::GetInstance().IsKeyHeld(VK_LBUTTON))
		{
			if (mySelectedModelIndex >= 0 && myTimer >= mySelectDelay)
			{
				MoveObject();
			}
			else
			{
				CheckAABB3DCollision();
				myTimer += Global::GetDeltaTime();
			}
		}
		else
		{
			if (mySelectedModelIndex >= 0)
			{
				World::GetActiveScene()->myModels[mySelectedModelIndex]->SetBoundingBoxLineColor({ 1.0f,1.0f,0.0f , 1.0f });
			}

			mySelectedModelIndex = -1;
			myTimer = 0;
		}

		if (SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_LBUTTON) || SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_RBUTTON))
		{
			CheckRayNavmesh();
		}
	}

	void RaycastManager::Render()
	{
		if (Global::GetRenderer()->IsDebugModeOn())
		{
			Global::GetRenderer()->RenderLine(*myRaycastLine);

			if (myShouldRenderDebugSphere)
			{
				Global::GetRenderer()->RenderSphere(*myDebugSphere);
			}
		}
	}

	SimpleUtilities::Ray RaycastManager::GetScreenPointToRay(const Math::Vector2f& aPosition)
	{
		const auto camera = Global::GetGraphicsEngine()->GetCurrentCamera();

		const Math::Matrix4x4f projectionMatrix = camera->GetProjectionMatrix();
		const Math::Matrix4x4f viewMatrix = camera->GetViewMatrix();

		const Math::Vector2f resolution = Math::Vector2f(static_cast<float>(Global::GetResolution().x), static_cast<float>(Global::GetResolution().y));

		const float x = (2.0f * aPosition.x) / resolution.x - 1.0f;
		const float y = (1.0f - (2.0f * aPosition.y) / resolution.y) * -1.0f;

		Math::Vector4f clipCoords = { x, y, -1.0f, 1.0f };

		Math::Matrix4x4f invertedProjection = Math::Matrix4x4f::GetInverse(projectionMatrix);
		Math::Vector4f eyeCoords = invertedProjection * clipCoords;
		eyeCoords.z = -1.0f;
		eyeCoords.w = 0.0f;

		Math::Matrix4x4f invertedViewMatrix = Math::Matrix4x4f::GetInverse(viewMatrix);
		Math::Vector4f rayWorld = invertedViewMatrix * eyeCoords;

		Math::Vector3f mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
		mouseRay.Normalize();

		myRaycastLine->startPosition = camera->GetPosition();
		myRaycastLine->startPosition.x += 0.05f;
		myRaycastLine->endPosition = myRaycastLine->startPosition + mouseRay * 10000.0f;

		SU::Ray ray(camera->GetPosition(), mouseRay);
		return ray;
	}

	void RaycastManager::CheckAABB3DCollision()
	{
		SU::Ray ray = GetScreenPointToRay(SU::InputManager::GetInstance().GetMousePosition());

		const auto& models = World::GetActiveScene()->myModels;
		Math::Vector3f closetHitPoint;
		float closetDistance = FLT_MAX;
		int hitIndex = -1;

		for (unsigned int i = 0; i < models.size(); ++i)
		{
			const auto min = models[i]->GetBoundingBox().min;
			const auto max = models[i]->GetBoundingBox().max;
			const auto worldMatrix = models[i]->GetMatrix();

			Math::Vector4f minHomogeneous(min.x, min.y, min.z, 1.0f);
			Math::Vector4f maxHomogeneous(max.x, max.y, max.z, 1.0f);

			minHomogeneous = worldMatrix * minHomogeneous;
			maxHomogeneous = worldMatrix * maxHomogeneous;

			SU::AABB3D aabb3d;
			aabb3d.InitWithMinAndMax(minHomogeneous.AsVector3XYZ(), maxHomogeneous.AsVector3XYZ());

			if (Math::IntersectionAABB3DRay(aabb3d, ray, closetHitPoint))
			{
				const float distance = Math::Distance(ray.GetOrigin(), closetHitPoint);

				if (distance < closetDistance)
				{
					closetDistance = distance;
					hitIndex = i;
					myDebugSphere->position = closetHitPoint;
					myShouldRenderDebugSphere = true;
				}
			}
		}

		if (hitIndex >= 0)
		{
			mySelectedModelIndex = hitIndex;
		}
		else
		{
			mySelectedModelIndex = -1;
			myShouldRenderDebugSphere = false;
		}
	}

	void RaycastManager::CheckRayNavmesh()
	{
		SU::Ray ray = GetScreenPointToRay(SimpleUtilities::InputManager::GetInstance().GetMousePosition());
		Math::Vector3f intersectionPoint;
		Math::Vector3 closetPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		bool hasHit = false;

		Simple::Navmesh* navmesh = World::GetNavmesh();
		const auto& vertices = navmesh->GetNavmesh();
		const auto& nodes = navmesh->GetNodes();

		for (const Simple::Node& node : nodes)
		{

			if (Math::IntersectionPlaneRay(node.myPlane, ray, intersectionPoint))
			{
				//Check if Intersection Point is Inside This Triangle
				const Math::Vector3f v1 = vertices.myVertices[node.myIndices[0]];
				const Math::Vector3f v2 = vertices.myVertices[node.myIndices[1]];
				const Math::Vector3f v3 = vertices.myVertices[node.myIndices[2]];

				const float d = (v2.z - v3.z) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.z - v3.z);
				const float w1 = ((v2.z - v3.z) * (intersectionPoint.x - v3.x) + (v3.x - v2.x) * (intersectionPoint.z - v3.z)) / d;
				const float w2 = ((v3.z - v1.z) * (intersectionPoint.x - v3.x) + (v1.x - v3.x) * (intersectionPoint.z - v3.z)) / d;
				const float w3 = 1.0f - w1 - w2;

				const bool inside = w1 >= 0 && w2 >= 0 && w3 >= 0;

				if (inside)
				{
					hasHit = true;

					if (intersectionPoint.y > closetPoint.y)
					{
						closetPoint = intersectionPoint;
					}
				}
			}
		}

		if (hasHit)
		{
			Simple::Message<eEvent> raycast;

			auto& input = SimpleUtilities::InputManager::GetInstance();

			if (input.IsKeyPressed(VK_LBUTTON) == true && input.IsKeyPressed(VK_RBUTTON) == false)
				raycast.myType = Simple::eEvent::Raycast_LMB;
			else if (input.IsKeyPressed(VK_RBUTTON) == true && input.IsKeyPressed(VK_LBUTTON) == false)
				raycast.myType = Simple::eEvent::Raycast_RMB;
			else
				return;

			raycast.myData = closetPoint;

			World::GetEventmanager()->GetPostMaster().NotifyObservers(raycast);
		}
	}

	void RaycastManager::MoveObject()
	{
		auto& model = World::GetActiveScene()->myModels[mySelectedModelIndex];

		const auto mouseDelta = SimpleUtilities::InputManager::GetInstance().GetMouseDelta();
		const auto min = model->GetBoundingBox().min;
		const auto max = model->GetBoundingBox().max;
		const auto worldMatrix = model->GetMatrix();

		auto position = model->GetPosition();
		float x = position.x;
		float y = position.y;
		float z = position.z;

		Math::Vector4f minHomogeneous(min.x, min.y, min.z, 1.0f);
		Math::Vector4f maxHomogeneous(max.x, max.y, max.z, 1.0f);

		minHomogeneous = worldMatrix * minHomogeneous;
		maxHomogeneous = worldMatrix * maxHomogeneous;

		SU::AABB3D aabb3d;
		aabb3d.InitWithMinAndMax(minHomogeneous.AsVector3XYZ(), maxHomogeneous.AsVector3XYZ());

		const SU::Ray ray = GetScreenPointToRay(SU::InputManager::GetInstance().GetMousePosition());

		Math::Vector3f intersectionPoint;
		bool hit = Math::IntersectionAABB3DRay(aabb3d, ray, intersectionPoint);

		if (SimpleUtilities::InputManager::GetInstance().IsKeyHeld(VK_CONTROL))
		{
			y += mouseDelta.y * 0.02f;
		}
		else if (hit)
		{
			x += intersectionPoint.x - x;
			z += intersectionPoint.z - z;
		}

		if (Math::Distance(Math::Vector2f(model->GetPosition().x, model->GetPosition().z), Math::Vector2f(x, z)) > 0.1f)
		{
			position.x = x;
			position.z = z;
		}

		position.y = y;

		model->SetPosition(position);
		myDebugSphere->position = intersectionPoint;
		model->SetBoundingBoxLineColor({ 1.0f,0.0f,0.0f , 1.0f });
	}
}