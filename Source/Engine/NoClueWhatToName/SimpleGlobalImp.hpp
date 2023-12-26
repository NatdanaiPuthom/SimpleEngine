#pragma once

//Only For global.cpp

class Engine;
class Renderer;
class ModelFactory;

class SimpleGlobalEngineImpl final
{
private:
	friend class Engine;

	SimpleGlobalEngineImpl();

	static void SetEngine(Engine* aEngine);
	static void SetResolution(const SimpleUtilities::Vector2ui& aResolution);
	static void SetWindowSize(const SimpleUtilities::Vector2ui& aWindowSize);

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

class SimpleGlobalModelFactoryImpl final
{
private:
	friend class ModelFactory;
	static void SetModelFactory(ModelFactory* aModelFactory);
};