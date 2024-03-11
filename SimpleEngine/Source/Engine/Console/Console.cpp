#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Console/Console.hpp"

namespace Simple
{
	Console::Console()
	{
	}

	Console::~Console()
	{
#pragma warning( push )
#pragma warning( disable : 4996 )
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
#pragma warning( pop )
		FreeConsole();
	}

	void Console::Init()
	{
#pragma warning(push)
#pragma warning( disable : 4996 )
		AllocConsole();
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);

		setbuf(stdin, NULL);
		setbuf(stdout, NULL);
		setbuf(stderr, NULL);

		SetConsoleTitle(L"Simple makes thing easier");
#pragma warning( pop )
	}
}