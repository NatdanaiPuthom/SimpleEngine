#pragma once

class ImGuiEngine;
class GraphicsEngine;

namespace SimpleUtilities
{
	class Timer;
	class InputManager;
}

class Engine final
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE& hInstance, const int nCmdShow);

	bool BeginFrame();
	void EndFrame();
public:
	GraphicsEngine* GetGraphicsEngine();
	double GetTotalTime() const;
	float GetDeltaTime() const;
	HWND& GetHWND();
private:
	std::unique_ptr<HWND> SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
	void LoadSettingsFromJson();
private:
	SimpleUtilities::Vector2i myWindowSize;
	SimpleUtilities::InputManager* myInput;

	std::unique_ptr<GraphicsEngine> myGraphicsEngine;
	std::unique_ptr<SimpleUtilities::Timer> myTimer;
	std::unique_ptr<HWND> myHWND;
	std::unique_ptr<ImGuiEngine> myImGuiInterface;

#ifdef _DEBUG
	Console myConsole;
#endif
};
