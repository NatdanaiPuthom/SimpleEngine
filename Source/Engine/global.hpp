#pragma once

class GraphicsEngine;

namespace SimpleGlobal
{
	GraphicsEngine* GetGraphicsEngine();
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
	void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize);
}
