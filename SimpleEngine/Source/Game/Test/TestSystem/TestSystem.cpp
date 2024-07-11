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
			ECS::AnimatedComponent* animatedComponent = human->GetComponent<ECS::AnimatedComponent>();
			ECS::AnimationPlayerComponent* animationPlayerComponent = human->GetComponent<ECS::AnimationPlayerComponent>();

			if (animatedComponent != nullptr)
			{
				static bool doOnce = true;

				if (doOnce)
				{
					doOnce = false;

					const std::string absolutePathAnimation = SimpleUtilities::GetAbsolutePath("Assets\\Models\\Animations\\SimpleHuman3_Idle.fbx");
					const std::string absolutePathModel = SimpleUtilities::GetAbsolutePath("Assets\\Models\\AnimatedModels\\SimpleHuman3.fbx");

					animatedComponent->animation = Global::GetModelFactory()->LoadAnimationFBX(absolutePathAnimation.c_str());
					animatedComponent->skeleton = Global::GetModelFactory()->LoadSkeleton(absolutePathModel);

					if (animationPlayerComponent != nullptr)
					{
						animationPlayerComponent->animationPlayer.Init(animatedComponent->animation, animatedComponent->skeleton);
						animationPlayerComponent->animationPlayer.Play(true);
					}
				}
			}

			if (animationPlayerComponent != nullptr)
			{
				if (animatedComponent != nullptr)
				{
					animationPlayerComponent->animationPlayer.UpdateTest(animatedComponent->jointMatrices, animatedComponent);
				}
			}
		}
	}

	void TestSystem::Render()
	{
	}
}