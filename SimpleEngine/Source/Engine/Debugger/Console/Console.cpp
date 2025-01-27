#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Debugger/Console/Console.hpp"

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

		SetConsoleTitle(L"Simple, it's just that easy");
#pragma warning( pop )
	}

#ifndef _SIMPLE
	void Console::Print(const char* aText, const ConsoleTextColor aColor, const bool aShouldEndline)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
		GetConsoleScreenBufferInfo(hConsole, &defaultInfo);
		const WORD defaultAttributes = defaultInfo.wAttributes;

		switch (aColor)
		{
		case ConsoleTextColor::Red:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case ConsoleTextColor::Green:
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case ConsoleTextColor::Blue:
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case ConsoleTextColor::Yellow:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case ConsoleTextColor::White:
			SetConsoleTextAttribute(hConsole, defaultAttributes);
			break;
		default:
			SetConsoleTextAttribute(hConsole, defaultAttributes);
			break;
		}

		std::cout << aText;

		if (aShouldEndline)
		{
			std::cout << std::endl;
		}

		SetConsoleTextAttribute(hConsole, defaultAttributes);
	}
#else
	void Console::Print(const char* /*aText*/, const ConsoleTextColor /*aColor*/, const bool /*aShouldEndline*/)
	{
	}
#endif
}