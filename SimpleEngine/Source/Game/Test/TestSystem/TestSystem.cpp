#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/TestSystem/TestSystem.hpp"

namespace ECS
{
	TestSystem::TestSystem(EntityComponentSystem* aEntityComponentSystem) : System(aEntityComponentSystem)
	{
	}

	TestSystem::~TestSystem()
	{
	}

	void TestSystem::Init()
	{
	}

	void TestSystem::Update()
	{
		/*auto& a = myEntityComponentSystem->GetEntityIDsWithThisComponent<MeshComponent>();

		for (auto& b : a)
		{
			auto c = myEntityComponentSystem->GetEntity(b);
			auto e = c->GetComponent<TransformComponent>();
			auto t = e->transform.GetPosition();
			t.x += 10 * Global::GetDeltaTime();
			e->transform.SetPosition(t);
		}*/


		ECS::Entity human = myEntityComponentSystem->GetEntity(3);

		if (human != nullptr)
		{
			ECS::AnimationComponent* animationPlayerComponent = human->GetComponent<ECS::AnimationComponent>();

			if (animationPlayerComponent != nullptr)
			{
				animationPlayerComponent->animationPlayer.UpdateTest(animationPlayerComponent->jointMatrices, animationPlayerComponent);
			}
		}
	}

	void TestSystem::Render()
	{
	}

	std::unique_ptr<System> TestSystem::Clone(EntityComponentSystem* aEntityComponentSystem) const
	{
		return std::make_unique<TestSystem>(aEntityComponentSystem);
	}
}