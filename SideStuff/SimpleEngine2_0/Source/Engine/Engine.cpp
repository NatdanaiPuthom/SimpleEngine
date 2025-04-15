#include "Engine.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "Utility/Timer.hpp"
#include <Windows.h>

namespace Simple
{
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
		myTimer = new Timer();
		myConsole = new Console();

		myConsole->Init();
	}

	void Engine::Update()
	{
		myTimer->Update();
	}
}