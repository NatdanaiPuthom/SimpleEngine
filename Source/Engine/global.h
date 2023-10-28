#pragma once

class Engine;
class GraphicsEngine;

class SimplyGlobalImpl
{
	friend class Engine;
	SimplyGlobalImpl();
	static void SetEngine(Engine* aEngine);
};

namespace SimplyGlobal
{
	GraphicsEngine* GetGraphicsEngine();
	double GetTotalTime();
	float GetDeltaTime();
	HWND& GetHWND();
}
