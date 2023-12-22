#include "Engine/stdafx.h"
#include "Engine/SimpleUtilities/Timer.hpp"
#include "Engine/ImGuiEngine/ImGuiEngine.hpp"
#include <External/json.h>

#ifdef _DEBUG
#include "Engine/Console/Console.hpp"
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Engine::Engine()
	: myInput(nullptr)
	, myImGuiInterface(std::make_unique<ImGuiEngine>())
{
}

Engine::~Engine()
{
	myInput = nullptr;
}

void Engine::Init(HINSTANCE& hInstance, const int nCmdShow)
{
	LoadSettingsFromJson();

	myHWND = SetupMainWindow(hInstance, myWindowSize.x, myWindowSize.y);
	assert(myHWND && "Failed To Create Window");

#ifdef _DEBUG
	myConsole.Init();
#endif

	ShowWindow(*myHWND, nCmdShow);
	UpdateWindow(*myHWND);

	SimpleUtilities::InputManager::GetInstance().SetHWND(*myHWND);
	SimplyGlobalImpl::SetEngine(this);

	myGraphicsEngine = std::make_unique<GraphicsEngine>();
	const bool success = myGraphicsEngine->Init(myWindowSize.x, myWindowSize.y, *myHWND);
	assert(success && "Failed To Init Graphics Engine");

	myTimer = std::make_unique<SimpleUtilities::Timer>();
	myInput = &SimpleUtilities::InputManager::GetInstance();

	myImGuiInterface->Init();
}

void Engine::LoadSettingsFromJson()
{
	const std::string filename = SimpleUtilities::GetPath(SIMPLE_SETTINGS_FILENAME);
	std::ifstream file(filename);
	assert(file.is_open() && "Failed To Open File");

	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	const nlohmann::json& windowSize = json["game_settings"]["window_size"];

	myWindowSize.x = windowSize["x"];
	myWindowSize.y = windowSize["y"];
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
		L"SimpleEngine v8.2",
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
	myImGuiInterface->BeginFrame();

	SimplyGlobalImpl::UpdateFPSCounter();

	myTimer->Update();
	myInput->Update();

	return myGraphicsEngine->BeginFrame();
}

void Engine::EndFrame()
{
	//myGraphicsEngine->SetToBackBuffer(); //Disabled for now due to issue with ImGui::Image rendering

	myImGuiInterface->EndFrame();
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
