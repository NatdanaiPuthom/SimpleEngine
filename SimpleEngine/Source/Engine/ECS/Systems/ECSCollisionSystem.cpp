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

	void ECSCollisionSystem::Update(EntityComponentSystem*)
	{
	}

	void ECSCollisionSystem::Render(EntityComponentSystem*)
	{
		myRayCollider->GetShape<Ray>() = Global::GetMouseRay();

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

