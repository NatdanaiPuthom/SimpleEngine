#pragma once

#ifdef _DEBUG
#include "Console/Console.h"
#endif

class Timer;

class Engine
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE& hInstance, const int aWidth, const int aHeight);

	bool BeginFrame();
	void EndFrame();
public:
	GraphicsEngine* GetGraphicsEngine();
	double GetTotalTime() const;
	float GetDeltaTime() const;
	HWND& GetHWND();
private:
	std::unique_ptr<HWND> SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
private:
	SimpleUtilities::InputManager* myInput;

	std::unique_ptr<GraphicsEngine> myGraphicsEngine;
	std::unique_ptr<Timer> myTimer;
	std::unique_ptr<HWND> myHWND;

#ifdef _DEBUG
	Console myConsole;
#endif
};
