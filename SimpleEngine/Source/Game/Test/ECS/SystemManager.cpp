#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/SystemManager.hpp"

namespace ECS
{
	SystemManager::SystemManager()
		: myTimer(0.0f)
		, myFixedUpdate(1.0f / 60.0f)
	{
		mySystems.reserve(16);
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
		const bool shouldUpdate = myTimer > myFixedUpdate;

		if (myTimer > myFixedUpdate)
		{
			myTimer = 0.0f;
		}

		for (auto& system : mySystems)
		{
			system->EarlyUpdate();
		}

		for (auto& system : mySystems)
		{
			if (shouldUpdate)
			{
				system->FixedUpdate();
			}
		}

		for (auto& system : mySystems)
		{
			system->Update();
		}

		for (auto& system : mySystems)
		{
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