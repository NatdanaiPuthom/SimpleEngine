#pragma once

#ifdef _DEBUG
#include "Engine/Console/Console.h"
#endif

class ImGuiInterface;
class GraphicsEngine;

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
private:
	SimpleUtilities::InputManager* myInput;

	std::unique_ptr<GraphicsEngine> myGraphicsEngine;
	std::unique_ptr<SimpleUtilities::Timer> myTimer;
	std::unique_ptr<HWND> myHWND;
	std::unique_ptr<ImGuiInterface> myImGuiInterface;

#ifdef _DEBUG
	Console myConsole;
#endif
};
