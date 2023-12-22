#pragma once

class Engine;

class SimpleGlobalImpl
{
private:
	friend class Engine;

	SimpleGlobalImpl();

	static void SetEngine(Engine* aEngine);
	static void UpdateFPSCounter();
};
