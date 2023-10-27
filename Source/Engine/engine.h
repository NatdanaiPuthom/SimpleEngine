#pragma once

#ifdef _DEBUG
#include "Console/Console.h"
#endif

class Timer;
class GraphicsEngine;

class Engine
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE& hInstance, const int aWidth, const int aHeight);

	bool BeginFrame();
	void EndFrame();
public:
	float GetDeltaTime() const;
	double GetTotalTime() const;
	HWND& GetHWND();
private:
	HWND* SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
private:
	GraphicsEngine* myGraphicsEngine;
	Timer* myTimer;
	HWND* myHWND;

#ifdef _DEBUG
	Console myConsole;
#endif
};
