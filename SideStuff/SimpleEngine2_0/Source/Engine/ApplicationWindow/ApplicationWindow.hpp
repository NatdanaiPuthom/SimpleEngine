#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace Simple
{
	class ApplicationWindow final
	{
	public:
		static HWND SetUpMainWindow(HINSTANCE& aHandleInstance, const int aWidth, const int aHeight, const int aCmdShow);
		static void PeekAndDispatchMessages();

		static bool IsRunning();
		static void SetIsRunning(const bool aShouldRun);
	private:
		inline static bool  myIsRunning = false;
	};
}