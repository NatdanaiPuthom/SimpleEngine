#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

namespace Simple
{
	class Console final
	{
	public:
		Console();
		~Console();

		bool Init();

		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;

	private:
		bool myConsoleCreatedSuccessfully;
	};

	inline Console::Console()
		: myConsoleCreatedSuccessfully(false)
	{
	}

	inline Console::~Console()
	{
		if (myConsoleCreatedSuccessfully)
		{
			fclose(stdin);
			fclose(stdout);
			fclose(stderr);
			FreeConsole();
		}
	}

	inline bool Console::Init()
	{
		if (myConsoleCreatedSuccessfully == true)
		{
			return false;
		}

		if (AllocConsole())
		{
			freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

			setvbuf(stdin, NULL, _IONBF, 0);
			setvbuf(stdout, NULL, _IONBF, 0);
			setvbuf(stderr, NULL, _IONBF, 0);

			SetConsoleTitle(L"Simple, it's just that easy");

			myConsoleCreatedSuccessfully = true;
		}
		else
		{
			myConsoleCreatedSuccessfully = false;
		}

		return myConsoleCreatedSuccessfully;
	}
}

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif