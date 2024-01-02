#pragma once

namespace Simple
{
	class GraphicsEngine;
	class ModelFactory;
	class AudioManager;
}

namespace Drawer
{
	class Renderer;
}

namespace SimpleGlobal
{
	Simple::GraphicsEngine* GetGraphicsEngine();
	Simple::ModelFactory* GetModelFactory();
	Simple::AudioManager* GetAudioManager();

	Drawer::Renderer* GetRenderer();

	HWND& GetHWND();

	SimpleUtilities::Vector2ui GetResolution();
	SimpleUtilities::Vector2ui GetWindowSize();

	double GetTotalTime();
	float GetDeltaTime();
	int GetDrawCalls();
	int GetFPS();
	bool GetGameIsRunning();

	void SetGameIsRunning(const bool aShouldRun);
	void SetResolution(const SimpleUtilities::Vector2ui& aResolution);
	void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize, const bool aSetFullScreen = false);
}