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
		double GetTotalTime() const;
		float GetDeltaTime() const;
	private:
		std::unique_ptr<HWND> SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
		void LoadSettingsFromJson();
		void CheckAndCopySettingsFiles();
	private:
		std::unique_ptr<HWND> myHWND;
		std::unique_ptr<SimpleUtilities::Timer> myTimer;

		Simple::Console myConsole;
	};
}