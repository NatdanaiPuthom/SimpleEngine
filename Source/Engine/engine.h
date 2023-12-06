#pragma once

#ifdef _DEBUG
#include "Engine/Console/Console.h"
#endif

namespace SimpleUtilities
{
	class Timer;
}

class Engine
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE& hInstance, const int aWidth, const int aHeight, const int nCmdShow);
	void Render();

	bool BeginFrame();
	void EndFrame();
public:
	GraphicsEngine* GetGraphicsEngine();
	double GetTotalTime() const;
	float GetDeltaTime() const;
	HWND& GetHWND();
private:
	std::unique_ptr<HWND> SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
	void InitDearImGui();
private:
	SimpleUtilities::InputManager* myInput;

	std::unique_ptr<GraphicsEngine> myGraphicsEngine;
	std::unique_ptr<SimpleUtilities::Timer> myTimer;
	std::unique_ptr<HWND> myHWND;

#ifdef _DEBUG
	Console myConsole;
#endif
};
