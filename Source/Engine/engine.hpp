#pragma once

namespace SimpleUtilities
{
	class Timer;
}

class ImGuiEngine;
class GraphicsEngine;
class AudioManager;

class Engine final
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE& hInstance, const int nCmdShow);
	bool BeginFrame();
	void EndFrame();
public:
	HWND& GetHWND();
	GraphicsEngine* GetGraphicsEngine();
	double GetTotalTime() const;
	float GetDeltaTime() const;
private:
	std::unique_ptr<HWND> SetupMainWindow(HINSTANCE& hInstance, const int aWidth, const int aHeight);
	void LoadSettingsFromJson();
private:
	std::unique_ptr<HWND> myHWND;
	std::unique_ptr<SimpleUtilities::Timer> myTimer;
	std::unique_ptr<GraphicsEngine> myGraphicsEngine;
	std::unique_ptr<ImGuiEngine> myImGuiEngine;
	std::unique_ptr<AudioManager> myAudioManager;

	Console myConsole;
};