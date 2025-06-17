#include "Engine.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "Utility/Timer.hpp"
#include <Windows.h>

namespace Simple
{
	Engine* Engine::GetInstance()
	{
		if (sInstance == nullptr)
		{
			Console::Print("Engine instance has not been created. This is unintended behaviour.", ConsoleTextColor::Red);
			return nullptr;
		}

		return sInstance;
	}

	Engine* Engine::CreateInstance()
	{
		if (sInstance != nullptr)
		{
			Console::Print("An Engine instance has already been created. This is unintended behaviour.", ConsoleTextColor::Yellow);
			return sInstance;
		}

		sInstance = new Engine();
		Console::Print("Successfully created an Engine instance.", ConsoleTextColor::Green);

		return sInstance;
	}

	void Engine::DestroyInstance()
	{
		delete sInstance;
		sInstance = nullptr;
	}

	Engine::Engine()
		: myTimer(nullptr)
		, myConsole(nullptr)
	{
	}

	Engine::~Engine()
	{
		delete myTimer;
		delete myConsole;

		myTimer = nullptr;
		myConsole = nullptr;
	}

	void Engine::Init()
	{
		myConsole = new Console();
		myTimer = new Timer();

		myConsole->Init();
	}

	void Engine::Update()
	{
		myTimer->Update();
	}
}