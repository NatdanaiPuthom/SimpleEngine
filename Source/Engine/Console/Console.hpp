#pragma once
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

		void Init();

		// Delete copy constructor and copy assignment operator
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		// Default move constructor and move assignment operator
		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;
	};
}