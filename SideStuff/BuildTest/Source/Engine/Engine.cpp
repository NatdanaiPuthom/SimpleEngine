#include "Engine.hpp"
#include "Utility/Console.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace Simple
{
	Engine::Engine() 
		: myHWND(nullptr)
		, myIsRunning(false)
		, myConsole(nullptr)
	{
	}

	Engine::~Engine()
	{;
		myIsRunning = false;

		delete myConsole;

		DestroyWindow(static_cast<HWND>(myHWND));

		myHWND = nullptr;
		myConsole = nullptr;
	}

	void Engine::Init(HINSTANCEPtr aHInstance, const int aCmdShow)
	{
#ifndef _SIMPLE
		myConsole = new Console();
		myConsole->Init();
#endif
		aHInstance; aCmdShow;

		myIsRunning = true;
	}

	void Engine::Update()
	{
		myTimer.Update();
	}

	bool Engine::IsRunning() const noexcept
	{
		return myIsRunning;
	}
}