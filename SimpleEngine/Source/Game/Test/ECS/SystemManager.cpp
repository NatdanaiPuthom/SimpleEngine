#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/SystemManager.hpp"

namespace ECS
{
	SystemManager::SystemManager()
		: myTimer(0.0f)
		, myFixedUpdate(1.0f / 60.0f)
	{
	}

	SystemManager::~SystemManager()
	{
	}

	void SystemManager::Init()
	{
	}

	void SystemManager::AddSystem(std::unique_ptr<System> aSystem)
	{
		mySystems.push_back(std::move(aSystem));
		mySystemPointer[typeid(*mySystems.back())] = mySystems.back().get();
	}

	void SystemManager::Update()
	{
		myTimer += Global::GetDeltaTime();

		for (auto& system : mySystems)
		{
			system->EarlyUpdate();

			if (myTimer > myFixedUpdate)
			{
				system->FixedUpdate();
				myTimer = 0.0f;
			}

			system->Update();
			system->LateUpdate();
		}
	}

	void SystemManager::Render()
	{
		for (auto& system : mySystems)
		{
			system->Render();
		}
	}
}