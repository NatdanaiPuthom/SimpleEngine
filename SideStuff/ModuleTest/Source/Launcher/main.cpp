#define WIN32_LEAN_AND_MEAN //NOTE(v1.0.0): Fuck you Microsoft
#include <Windows.h>
#include <cassert>

import MOD_Engine;

using namespace Simple;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	Engine engine;

	bool success = engine.Init(hInstance, nCmdShow);
	assert(success && "Failed to init Engine.");

	while (engine.IsRunning())
	{
		engine.Update();
	}

	return 0;
}
