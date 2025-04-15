#include "Engine/Engine.hpp"
#include "Engine/ApplicationWindow/ApplicationWindow.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	HWND hwnd = Simple::ApplicationWindow::SetUpMainWindow(hInstance, 1280, 720, nCmdShow);

	if (hwnd)
	{
		Simple::ApplicationWindow::SetIsRunning(true);
	}

	Simple::Engine engine;
	engine.Init();

	while (Simple::ApplicationWindow::IsRunning())
	{
		Simple::ApplicationWindow::PeekAndDispatchMessages();
		engine.Update();
	}

	return 0;
}