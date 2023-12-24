#pragma once

//Only For global.cpp

class Engine;
class Mesh;

class SimpleGlobalEngineImpl final
{
private:
	friend class Engine;

	SimpleGlobalEngineImpl();

	static void SetEngine(Engine* aEngine);
	static void UpdateFPSCounter();
	static void ResetDrawCalls();
};

class SimpleGlobalMeshImpl final
{
private:
	friend class Mesh;
	SimpleGlobalMeshImpl();

	static void IncreaseDrawCall();
};
