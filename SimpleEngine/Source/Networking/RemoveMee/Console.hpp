#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <Windows.h>
#include <iostream>

namespace Simple
{
	class Console
	{
	public:
		Console()
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

			SetConsoleTitle(L"Simple, it's just that easy");
#pragma warning( pop )
		}

		~Console()
		{
#pragma warning( push )
#pragma warning( disable : 4996 )
			fclose(stdin);
			fclose(stdout);
			fclose(stderr);
#pragma warning( pop )
			FreeConsole();
		}
	};
}