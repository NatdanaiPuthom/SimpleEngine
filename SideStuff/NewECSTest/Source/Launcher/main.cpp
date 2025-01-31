#include "EntityComponentSystem/EntityComponentSystem.hpp"
#include "EntityComponentSystem/Reflection/Reflection.hpp"

#include <iostream>
#include <chrono>

//static void ResetPosition(Position& aPosition, float x, float y, float z)
//{
//	aPosition.x = x;
//	aPosition.y = y;
//	aPosition.z = z;
//}

struct Test
{
	int value;
	int value2;
};

REGISTER_COMPONENT_PROPERTY(Test, &Test::value2)


using namespace Simple;

int main()
{
	ComponentRegistry::GetInstance()->DestroyInstance();





	/*EntityComponentSystem ecs;
	EntityID entityID1 = ecs.CreateEntity();

	auto a = ecs.GetComponent<Identifier>(entityID1);

	if (a != nullptr)
	{
		std::cout << a->name << std::endl;
	}*/




	/*Simple::ComponentManager componentManager;

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

	std::cout << "Time took to Remove: " << time << std::endl;*/

	return 0;
}