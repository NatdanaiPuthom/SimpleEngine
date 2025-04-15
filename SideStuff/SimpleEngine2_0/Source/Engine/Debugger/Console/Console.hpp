#pragma once
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

namespace Simple
{
	enum class ConsoleTextColor
	{
		White,
		Red,
		Green,
		Blue,
		Yellow
	};

	class Console final
	{
	public:
		Console();
		~Console();

		void Init();

		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;

		static void Print(const char* aText, const ConsoleTextColor aColor, const bool aShouldEndline = true);
	};
}