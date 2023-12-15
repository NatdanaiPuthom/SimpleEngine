#pragma once

class Engine;
class GraphicsEngine;

class SimplyGlobalImpl
{
	friend class Engine;
	SimplyGlobalImpl();
	static void SetEngine(Engine* aEngine);
	static void UpdateFPSCounter();
};

namespace SimplyGlobal
{
	GraphicsEngine* GetGraphicsEngine();
	double GetTotalTime();
	float GetDeltaTime();
	int GetFPS();
	HWND& GetHWND();
}
