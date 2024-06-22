#define WIN32_LEAN_AND_MEAN //NOTE(v11.2.1): Fuck you Microsoft
#include "Networking/Console/Console.hpp"
#include "Networking/Server/Server.hpp"
#include <Windows.h>
#include <cassert>

std::atomic<bool> localIsRunning = true;

int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int /*nCmdShow*/)
{
	Simple::Console console;
	Simple::Server server;

	if (server.Init() == false)
	{
		assert(false && "Failed to init server");
		return -1;
	}

	while (localIsRunning)
	{
		if (server.Update() == false)
		{
			break;
		}
	}

	return 0;
}