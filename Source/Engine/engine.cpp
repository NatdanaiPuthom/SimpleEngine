#include "stdafx.h"
#include <cassert>
#include "engine.h"
#include "MemoryTracker/MemoryTracker.h"
#include "Graphics/GraphicsEngine.h"
#include "Timer/Timer.h"
#include "global.h"

#ifdef _DEBUG
#include "Console/Console.h"
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Engine::Engine()
	: myHWND(nullptr)
	, myGraphicsEngine(nullptr)
	, myTimer(nullptr)
{
	SimpleTracker::MemoryTrackingSettings memoryTrackerSettings = {};
	memoryTrackerSettings.myShouldStoreStackTraces = false;
	memoryTrackerSettings.myShouldTrackAllAllocations = true;
	SimpleTracker::StartMemoryTracking(memoryTrackerSettings);
}

Engine::~Engine()
{
	if (myHWND != nullptr)
		delete myHWND;

	if (myGraphicsEngine != nullptr)
		delete myGraphicsEngine;

	if (myTimer != nullptr)
		delete myTimer;

	myHWND = nullptr;
	myGraphicsEngine = nullptr;
	myTimer = nullptr;

	SimpleTracker::StopMemoryTrackingAndPrint();
}

void Engine::Init(HINSTANCE& hInstance, const int aWidth, const int aHeight)
{
	myHWND = SetupMainWindow(hInstance, aWidth, aHeight);
	assert(myHWND && "Failed To Create Window");

	SimplyGlobalImpl::SetEngine(this);

#ifdef _DEBUG
	myConsole.Init();
#endif

	ShowWindow(*myHWND, 1);
	UpdateWindow(*myHWND);

	myGraphicsEngine = new GraphicsEngine();
	bool success = myGraphicsEngine->Init(aHeight, aWidth, *myHWND);
	assert(success && "Failed To Init Graphics Engine");

	myTimer = new Timer();
}

HWND* Engine::SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight)
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

	HWND* hwnd = new HWND;
	*hwnd = CreateWindow(
		L"Natdanai",
		L"SimpleEngine",
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

bool Engine::BeginFrame()
{
	myTimer->Update();
	return myGraphicsEngine->BeginFrame();
}

void Engine::EndFrame()
{
	myGraphicsEngine->Render();
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
