#include "Engine/Engine.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	Simple::Engine engine;
	engine.Init(hInstance, nCmdShow);

	while (engine.IsRunning())
	{
		engine.Update();
	}

	return 0;
}