#include "Engine/Precomplier/stdafx.h"
#include "Engine/SimpleUtilities/Timer.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace Simple
{
	Engine::Engine()
		: myImGuiEngine(std::make_unique<Simple::ImGuiEngine>())
	{
	}

	Engine::~Engine()
	{
	}

	void Engine::Init(HINSTANCE& hInstance, const int nCmdShow)
	{
		LoadSettingsFromJson();

		const SimpleUtilities::Vector2ui windowSize = SimpleGlobal::GetWindowSize();

		myHWND = SetupMainWindow(hInstance, windowSize.x, windowSize.y);
		assert(myHWND && "Failed To Create Window");

#ifndef _SIMPLE
		myConsole.Init();
#endif

		ShowWindow(*myHWND, nCmdShow);
		UpdateWindow(*myHWND);

		SimpleUtilities::InputManager::GetInstance().SetHWND(*myHWND);
		Impl::SimpleGlobalEngine::SetEngine(this);

		myGraphicsEngine = std::make_unique<GraphicsEngine>();
		const bool success = myGraphicsEngine->Init(windowSize, *myHWND);
		assert(success && "Failed To Init Graphics Engine");

		myTimer = std::make_unique<SimpleUtilities::Timer>();
		myAudioManager = std::make_unique<AudioManager>();

		myImGuiEngine->Init();
		myAudioManager->Init();
	}

	void Engine::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetPath(SIMPLE_SETTINGS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json& windowSizeJson = json["game_settings"]["window_size"];
		const nlohmann::json& resolutionJson = json["game_settings"]["resolution"];

		SimpleUtilities::Vector2ui windowSize;
		windowSize.x = windowSizeJson["x"];
		windowSize.y = windowSizeJson["y"];

		SimpleUtilities::Vector2ui resolution;
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
			L"SimpleEngine v9.0",
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

	bool Engine::BeginFrame()
	{
		myImGuiEngine->BeginFrame();

		Impl::SimpleGlobalEngine::UpdateFPSCounter();
		Impl::SimpleGlobalEngine::ResetDrawCalls();

		myTimer->Update();
		myAudioManager->Update();

		SimpleUtilities::InputManager::GetInstance().Update();

		return myGraphicsEngine->BeginFrame();
	}

	void Engine::EndFrame()
	{
		myImGuiEngine->EndFrame();
		myGraphicsEngine->EndFrame();
	}

	float Engine::GetDeltaTime() const
	{
		return myTimer->GetDeltaTime();
	}

	double Engine::GetTotalTime() const
	{
		return myTimer->GetTotalTime();
	}

	HWND& Engine::GetHWND()
	{
		return *myHWND;
	}

	GraphicsEngine* Engine::GetGraphicsEngine()
	{
		return myGraphicsEngine.get();
	}
}