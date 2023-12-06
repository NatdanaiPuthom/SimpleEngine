#include "stdafx.h"
#include "Engine/SimpleUtilities/Timer.h"
#include "Engine/global.h"

#include <External/dearimgui/imgui/imgui.h>
#include <External/dearimgui/imgui/imgui_impl_win32.h>
#include <External/dearimgui/imgui/imgui_impl_dx11.h>

#ifdef _DEBUG
#include "Engine/Console/Console.h"
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Engine::Engine()
	: myInput(nullptr)
{
}

Engine::~Engine()
{
	myInput = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Engine::Init(HINSTANCE& hInstance, const int aWidth, const int aHeight, const int nCmdShow)
{
	myHWND = SetupMainWindow(hInstance, aWidth, aHeight);
	assert(myHWND && "Failed To Create Window");

	SimplyGlobalImpl::SetEngine(this);

#ifdef _DEBUG
	myConsole.Init();
#endif

	ShowWindow(*myHWND, nCmdShow);
	UpdateWindow(*myHWND);

	myGraphicsEngine = std::make_unique<GraphicsEngine>();
	bool success = myGraphicsEngine->Init(aHeight, aWidth, *myHWND);
	assert(success && "Failed To Init Graphics Engine");

	myTimer = std::make_unique<SimpleUtilities::Timer>();
	myInput = &SimpleUtilities::InputManager::GetInstance();

	InitDearImGui();
}

void Engine::Render()
{
	myGraphicsEngine->Render();
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
	assert(wcex.hIcon != NULL && "Failed to load icon");

	if (!RegisterClassExW(&wcex))
	{
		MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
		return nullptr;
	}

	std::unique_ptr<HWND> hwnd = std::make_unique<HWND>();
	*hwnd = CreateWindow(
		L"Natdanai",
		L"SimpleEngine v4.4",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		aWidth,
		aHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return hwnd;
}

void Engine::InitDearImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(*myHWND);
	ImGui_ImplDX11_Init(myGraphicsEngine->GetDevice().Get(), myGraphicsEngine->GetContext().Get());
}

bool Engine::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	myTimer->Update();
	myInput->Update();

	return myGraphicsEngine->BeginFrame();
}

void Engine::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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
