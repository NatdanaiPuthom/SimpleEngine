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
		auto& a = myEntityComponentSystem->GetEntityIDsWithThisComponent<MeshComponent>();

		for (auto& b : a)
		{
			auto c = myEntityComponentSystem->GetEntity(b);
			auto e = c->GetComponent<TransformComponent>();
			auto t = e->transform.GetPosition();
			t.x += 10 * Global::GetDeltaTime();
			e->transform.SetPosition(t);
		}
	}

	void TestSystem::Render()
	{
	}
}