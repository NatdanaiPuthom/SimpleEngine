#include "Engine/Precomplied/EnginePch.hpp"
#include "ECSCollisionSystem.hpp"
#include "SimpleUtilities/DebugShapes.hpp"
#include "Engine/engine.hpp"
#include "Global.hpp"
#include "Collision/CollisionFunctions.hpp"

using namespace Simple;

namespace ECS
{
	ECSCollisionSystem::ECSCollisionSystem()
	{
	}

	ECSCollisionSystem::~ECSCollisionSystem()
	{
	}

	void ECSCollisionSystem::Init(EntityComponentSystem*)
	{

		myKnifeCollider = &myCollisionSystem.CreateCollider(Sphere(Math::Vector3f(0, 7, 0), 5.f), eCollisionLayer::Interactable, eCollisionLayer::None);
		myEnemyCollider = &myCollisionSystem.CreateCollider(AABB3D(Math::Vector3f(0, 0, 0), Math::Vector3f(5, 5, 3)), eCollisionLayer::Enemy, eCollisionLayer::Player | eCollisionLayer::Wall);
		myRayCollider = &myCollisionSystem.CreateCollider(Ray(), eCollisionLayer::Player, eCollisionLayer::Interactable | eCollisionLayer::Enemy);

		myKnifeCollider->SetColor(Colors::Green);
		myEnemyCollider->SetColor(Colors::Blue);
		myRayCollider->SetColor(Colors::Red);

		myKnifeCollider->SetOnEnterCallback(std::bind(&ECSCollisionSystem::OnKnifePickup, this, std::placeholders::_1));
		myKnifeCollider->SetOnOverlapCallback(std::bind(&ECSCollisionSystem::OnKnifeHovered, this, std::placeholders::_1));
		myKnifeCollider->SetOnExitCallback([](Collider*) -> void { std::cout << "Knife exit!" << std::endl; });
		//myEnemyCollider->SetOnEnterCallback([](Collider*) -> void { std::cout << "Enemy Collided!" << std::endl; });
		//myRayCollider->SetOnEnterCallback([](Collider*) -> void { std::cout << "Ray Collided!" << std::endl; });
	}

	void ECSCollisionSystem::Update(EntityComponentSystem* )
	{
		

	}

	void ECSCollisionSystem::Render(EntityComponentSystem* aEntityComponentSystem)
	{
		myRayCollider->GetShape<Ray>() = Global::GetMouseRay();

		const std::unordered_set<EntityID>& entitiesID = aEntityComponentSystem->GetEntityIDsWithThisComponent<ColliderComponent>();

		std::vector<EntityID> ids;

		for (const EntityID entityID : entitiesID)
		{
			ids.push_back(entityID);;
		}


		Entity stella = aEntityComponentSystem->GetEntity(ids[0]);
		Entity cube = aEntityComponentSystem->GetEntity(ids[1]);

		ColliderComponent* stellaCollider = stella.GetComponent<ColliderComponent>();
		ColliderComponent* cubeCollider = cube.GetComponent<ColliderComponent>();

		MeshComponent* stellaMesh = stella.GetComponent<MeshComponent>();
		MeshComponent* cubeMesh = cube.GetComponent<MeshComponent>();

		TransformComponent* stellaTransform = stella.GetComponent<TransformComponent>();
		TransformComponent* cubeTransform = cube.GetComponent<TransformComponent>();

		AABB3D a1;
		AABB3D a2;
		Sphere a3;


		stellaTransform->transform.LookAt(cubeTransform->transform.GetPosition());

		a1.InitWithMinAndMax(stellaMesh->mesh->myBoundingBox.min * stellaTransform->transform.GetScale() + stellaTransform->transform.GetPosition(), stellaMesh->mesh->myBoundingBox.max * stellaTransform->transform.GetScale() + stellaTransform->transform.GetPosition());
		a2.InitWithMinAndMax(cubeMesh->mesh->myBoundingBox.min + cubeTransform->transform.GetPosition(), cubeMesh->mesh->myBoundingBox.max + cubeTransform->transform.GetPosition());
		a3.SetRadius(5);
		a3.SetCenter(cubeTransform->transform.GetPosition());

		stellaCollider->collider.myShape = a1;
		cubeCollider->collider.myShape = a3;

		if (stellaCollider->collider.DoesCollideWith(cubeCollider->collider.myShape))
		{
			stellaCollider->collider.SetColor(Colors::Red);
			cubeCollider->collider.SetColor(Colors::Red);
		}
		else
		{
			stellaCollider->collider.SetColor(Colors::Green);
			cubeCollider->collider.SetColor(Colors::Green);
		}


		stellaCollider->collider.Render();
		cubeCollider->collider.Render();

		/*myCollisionSystem.RenderColliders();

		myCollisionSystem.ProcessCollisions();*/

		//std::vector<Collider*> colliders = myCollisionSystem.PollCollisions(Ray());
	}

	std::unique_ptr<System> ECSCollisionSystem::Clone() const
	{
		return std::make_unique<ECSCollisionSystem>(*this);
	}


	void ECSCollisionSystem::OnKnifePickup(Collider*)
	{
		std::cout << "Knife pickup!" << std::endl;
	}

	void ECSCollisionSystem::OnKnifeHovered(Collider*)
	{
		std::cout << "Knife hovered!" << std::endl;
	}
}

