#include "Engine.hpp"
#include "Utility/Timer.hpp"
#include "Utility/ApplicationWindow/ApplicationWindow.hpp"
#include <Windows.h>

namespace Simple
{
	Engine::Engine()
		: myTimer(nullptr)
		, myIsRunning(false)
	{
	}

	Engine::~Engine()
	{
		delete myTimer;

		myIsRunning = false;
	}

	void Engine::Init(void* aHandleInstance, const int aCmdShow)
	{
		myTimer = new Timer();

		HINSTANCE hInstance = static_cast<HINSTANCE>(aHandleInstance);
		HWND hwnd = ApplicationWindow::SetUpMainWindow(hInstance, 1280, 720, aCmdShow);

		if (hwnd)
		{
			myIsRunning = true;
		}
	}

	void Engine::Update()
	{
		myTimer->Update();
	}

	bool Engine::IsRunning() const
	{
		return myIsRunning;
	}
}