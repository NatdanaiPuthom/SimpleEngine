#pragma once
#include "Engine/Debugger/Console/StreamBuffer.hpp"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

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
		Console() = default;
		~Console();

		void Init();

		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;

		//NOTE: Capture logs before the Engine is successfully initialize.
		static void PreInit();

		static void Print(const char* aText, const ConsoleTextColor aColor = ConsoleTextColor::White, const bool aShouldEndline = true);
	private:
		void CleanUp();
	private:
		inline static CaptureStreamBuffer* myCaptureBuffer = new CaptureStreamBuffer();
		inline static std::ostream myCaptureStream{ myCaptureBuffer };
		inline static std::streambuf* myOriginalCoutBuffer = nullptr;
	};
}