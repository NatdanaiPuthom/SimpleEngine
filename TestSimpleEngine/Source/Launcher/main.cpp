#define WIN32_LEAN_AND_MEAN //NOTE(v1.0.0): Fuck you Microsoft
#include <Windows.h>

import MOD_Engine;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	Simple::Engine engine;
	engine.Init(hInstance, nCmdShow);

	while (engine.IsRunning() == true)
	{
		engine.Update();
	}

	return 0;
}
