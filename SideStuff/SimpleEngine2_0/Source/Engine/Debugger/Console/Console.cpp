#include "Console.hpp"
#include <Windows.h>
#include <iostream>

namespace Simple
{
	Console::~Console()
	{
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);

		FreeConsole();
		CleanUp();
	}

	void Console::PreInit()
	{
		myOriginalCoutBuffer = std::cout.rdbuf(myCaptureBuffer);
	}

	void Console::Init()
	{
#ifdef _RETAIL
		//TO-DO: Log somewhere else
#else
		if (AllocConsole())
		{
			freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);

			std::cin.clear();
			std::cout.clear();
			std::cerr.clear();

			setvbuf(stdin, NULL, _IONBF, 0);
			setvbuf(stdout, NULL, _IONBF, 0);
			setvbuf(stderr, NULL, _IONBF, 0);

			std::cout.rdbuf(myOriginalCoutBuffer);

			const std::string capturedContent = myCaptureBuffer->GetContent();

			if (capturedContent.empty() == false)
			{
				std::cout << capturedContent << std::flush;
			}

			CleanUp();

			SetConsoleTitle(L"Simple, it's just that easy");
		}
#endif
	}

	void Console::Print(const char* aText, const ConsoleTextColor aColor, const bool aShouldEndline)
	{
#ifdef _RETAIL
		(void)aText;
		(void)aColor;
		(void)aShouldEndline;
		return;
#else
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
#endif		
	}

	void Console::CleanUp()
	{
		delete myCaptureBuffer;

		myCaptureBuffer = nullptr;
		myOriginalCoutBuffer = nullptr;
	}
}