#include "EntityComponentSystem/ComponentManager.hpp"
#include <iostream>
#include <chrono>
#include <string>

namespace Simple
{
	class EntityManager
	{
	public:
	};
}

struct Position
{
	float x;
	float y;
	float z;
};

//static void ResetPosition(Position& aPosition, float x, float y, float z)
//{
//	aPosition.x = x;
//	aPosition.y = y;
//	aPosition.z = z;
//}

int main()
{
	Simple::ComponentManager componentManager;

	constexpr size_t size = 10;

	std::chrono::high_resolution_clock::time_point myStartTimerTime = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < size; ++i)
	{
		componentManager.AddComponent<Position>(Simple::EntityID(i));
	}

	double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTimerTime).count();

	std::cout << "Time took to Add: " << time << std::endl;

	myStartTimerTime = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < size; ++i)
	{
		componentManager.RemoveComponent<Position>(Simple::EntityID(i));
	}
	time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myStartTimerTime).count();

	std::cout << "Time took to Remove: " << time << std::endl;

	return 0;
}