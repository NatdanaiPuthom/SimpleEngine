module;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN //NOTE(v1.0.0): Fuck you Microsoft
#endif

#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

export module MOD_Console;

export namespace Simple
{
	class Console
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
		bool m_ConsoleCreatedSuccessfully = false;
	};

	Console::Console()
	{
	}

	Console::~Console()
	{
		if (m_ConsoleCreatedSuccessfully)
		{
#pragma warning(push)
#pragma warning(disable : 4996)
			fclose(stdin);
			fclose(stdout);
			fclose(stderr);
#pragma warning(pop)
			FreeConsole();
		}
	}

	bool Console::Init()
	{
		if (m_ConsoleCreatedSuccessfully == true)
		{
			return false;
		}

#pragma warning(push)
#pragma warning(disable : 4996)
		if (AllocConsole())
		{
			freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

			setbuf(stdin, NULL);
			setbuf(stdout, NULL);
			setbuf(stderr, NULL);

			SetConsoleTitle(L"Simple, it's just that easy");

			m_ConsoleCreatedSuccessfully = true;
		}
		else
		{
			m_ConsoleCreatedSuccessfully = false;
		}
#pragma warning(pop)

		return m_ConsoleCreatedSuccessfully;
	}
}

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
