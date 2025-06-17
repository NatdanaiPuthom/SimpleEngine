#include "Engine/Debugger/MemoryTracker/MemoryTracker.h"
#include "Engine/Engine.hpp"
#include "Engine/ApplicationWindow/ApplicationWindow.hpp"
#include "Engine/Debugger/Console/Console.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	Simple::Console::PreInit();

	HWND hwnd = Simple::ApplicationWindow::SetUpMainWindow(hInstance, 1280, 720, nCmdShow);

	if (hwnd)
	{
		Simple::ApplicationWindow::SetIsRunning(true);
	}

	Simple::Engine* engine = Simple::Engine::CreateInstance();
	engine->Init();

	while (Simple::ApplicationWindow::IsRunning())
	{
		Simple::ApplicationWindow::PeekAndDispatchMessages();
	}

	Simple::Engine::DestroyInstance();

	return 0;
}