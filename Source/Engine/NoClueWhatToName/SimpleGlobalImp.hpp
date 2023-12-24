#pragma once

//Only For global.cpp

class Engine;
class Renderer;

class SimpleGlobalEngineImpl final
{
private:
	friend class Engine;

	SimpleGlobalEngineImpl();

	static void SetEngine(Engine* aEngine);
	static void UpdateFPSCounter();
	static void ResetDrawCalls();
};

class SimpleGlobalRendererImpl final
{
private:
	friend class Renderer;
	SimpleGlobalRendererImpl();

	static void IncreaseDrawCall();
	static void SetRenderer(Renderer* aRenderer);
};
