#pragma once

class GraphicsEngine;

namespace SimpleGlobal
{
	GraphicsEngine* GetGraphicsEngine();
	HWND& GetHWND();

	double GetTotalTime();
	float GetDeltaTime();
	int GetFPS();
	bool GetGameIsRunning();

	void SetGameIsRunning(const bool aShouldRun);
}
