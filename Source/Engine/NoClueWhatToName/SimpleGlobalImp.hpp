#pragma once

//Only For global.cpp

class Engine;

class SimpleGlobalImpl final
{
private:
	friend class Engine;

	SimpleGlobalImpl();

	static void SetEngine(Engine* aEngine);
	static void UpdateFPSCounter();
};
