#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Engine.hpp"
#include "Engine/SimpleUtilities/Timer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Audio/AudioManager.h"
#include <External/nlohmann/json.hpp>
#include <fstream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace Simple
{
	Engine::Engine()
	{
	}

	Engine::~Engine()
	{
	}

	void Engine::SetGlobalPointerToThis() 
	{
		Impl::SimpleGlobalEngine::SetEngine(this);
	}

	void Engine::Init(HINSTANCE& hInstance, const int nCmdShow)
	{
		myTimer = std::make_unique<SimpleUtilities::Timer>();

		LoadSettingsFromJson();

		const Math::Vector2ui windowSize = Global::GetWindowSize();

		myHWND = SetupMainWindow(hInstance, windowSize.x, windowSize.y);
		assert(myHWND && "Failed To Create Window");

#ifndef _SIMPLE
		myConsole.Init();
#endif

		ShowWindow(*myHWND, nCmdShow);
		UpdateWindow(*myHWND);
	}

	void Engine::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_GAME_SETTINGS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json& windowSizeJson = json["game_settings"]["window_size"];
		const nlohmann::json& resolutionJson = json["game_settings"]["resolution"];

		Math::Vector2ui windowSize;
		windowSize.x = windowSizeJson["x"];
		windowSize.y = windowSizeJson["y"];

		Math::Vector2ui resolution;
		resolution.x = resolutionJson["x"];
		resolution.y = resolutionJson["y"];

		Impl::SimpleGlobalEngine::SetResolution(resolution);
		Impl::SimpleGlobalEngine::SetWindowSize(windowSize);
	}

	std::unique_ptr<HWND> Engine::SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszClassName = L"Natdanai";
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
		wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(101));

		assert(wcex.hIcon != NULL && "Failed to load icon");
		assert(wcex.hIconSm != NULL && "Failed to load small icon");

		wcex.style &= ~CS_DROPSHADOW;

		if (!RegisterClassExW(&wcex))
		{
			MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
			return nullptr;
		}

		RECT wr = {};
		wr.left = 0;
		wr.right = aWidth + wr.left;
		wr.top = 0;
		wr.bottom = aHeight + wr.top;

		AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		std::unique_ptr<HWND> hwnd = std::make_unique<HWND>();
		*hwnd = CreateWindow(
			L"Natdanai",
			L"SimpleEngine v9.22.2",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr);

		return hwnd;
	}

	void Engine::Update()
	{
		myTimer->Update();
		SimpleUtilities::InputManager::GetInstance().Update();
		AudioManager::GetInstance().Update();
	}

	float Engine::GetDeltaTime() const
	{
		return myTimer->GetDeltaTime();
	}

	double Engine::GetTotalTime() const
	{
		return myTimer->GetTotalTime();
	}

	HWND& Engine::GetEngineHWND()
	{
		return *myHWND;
	}
}