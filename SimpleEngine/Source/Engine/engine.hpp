#pragma once
#include "Engine/Console/Console.hpp"
#include <Windows.h>
#include <memory>

namespace SimpleUtilities
{
	class Timer;
}

namespace Simple
{
	class Engine final
	{
	public:
		Engine();
		~Engine();

		void Init(HINSTANCE& hInstance, const int nCmdShow);
		void Update();

	public:
		void SetGlobalPointerToThis();
	public:
		HWND& GetEngineHWND();
		HCURSOR& GetCustomCursor();
		DWORD GetOriginalWindowStyle() const;
		double GetTotalTime() const;
		float GetDeltaTime() const;
	private:
		HWND SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
		void LoadSettingsFromJson();
		void CheckAndCopySettingsFiles();
	private:
		HWND myHWND;
		std::unique_ptr<SimpleUtilities::Timer> myTimer;

		HCURSOR myCustomCursor;
		DWORD myOriginalWindowStyle;

		Simple::Console myConsole;
	};
}