#include "Engine/Precomplied/EnginePch.hpp"
#include "TestSystem.hpp"

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
			auto animated = human->GetComponent<ECS::AnimatedComponent>();

			if (animated != nullptr)
			{
				static bool doOnce = true;
				if (doOnce)
				{
					doOnce = false;

					const std::string absolutePathAnimation = SimpleUtilities::GetAbsolutePath("Assets\\Models\\Animations\\SimpleHuman3_Idle.fbx");
					const std::string absolutePathModel = SimpleUtilities::GetAbsolutePath("Assets\\Models\\AnimatedModels\\SimpleHuman3.fbx");

					animated->animation = Global::GetModelFactory()->LoadAnimationFBX(absolutePathAnimation.c_str());
					animated->skeleton = Global::GetModelFactory()->LoadSkeleton(absolutePathModel);
					animated->shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::Unlit_Animated).get();
				}
			}
		}
	}

	void TestSystem::Render()
	{
	}
}