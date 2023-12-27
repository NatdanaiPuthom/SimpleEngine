#pragma once

class GraphicsEngine;
class Renderer;
class ModelFactory;

namespace SimpleGlobal
{
	GraphicsEngine* GetGraphicsEngine();
	Renderer* GetRenderer();
	ModelFactory* GetModelFactory();

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
	void SetBoundingBoxLineColor(const SimpleUtilities::Vector3f& aColor);
}
