#pragma once
#include "Engine/Debugger/Console/Console.hpp"
#include <Windows.h>
#include <memory>
#include <unordered_map>
#include <string>

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
		void SetCustomCursor(const std::string& aCursorName);
	public:
		HWND& GetEngineHWND();
		const HCURSOR& GetCurrentCustomCursor();
		const DWORD GetOriginalWindowStyle() const;
		double GetTotalTime() const;
		float GetDeltaTime() const;
		const std::unordered_map<std::string, const HCURSOR>& GetLoadedCustomCursors() const;
	private:
		HWND SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
		void LoadSettingsFromJson();
		void LoadCustomCursors();
		void CheckAndCopySettingsFiles();
	private:
		std::unordered_map<std::string, const HCURSOR> myCustomCursors;
		const HCURSOR* myCurrentCustomCursor;

		std::unique_ptr<SimpleUtilities::Timer> myTimer;

		HWND myHWND;
		DWORD myOriginalWindowStyle;

		Simple::Console myConsole;
	};
}