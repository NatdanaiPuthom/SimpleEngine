#include "EntityComponentSystem/EntityComponentSystem.hpp"
#include "EntityComponentSystem/Reflection/Reflection.hpp"

#include <iostream>
#include <chrono>

COMPONENT(Test)
{
	int value;
	int value2;
};

REGISTER_COMPONENT_PROPERTY(Test, &Test::value)
REGISTER_COMPONENT_PROPERTY(Test, &Test::value2)


COMPONENT(HelloWorld)
{
	float testFloat;
	bool boo;
};

REGISTER_COMPONENT_PROPERTY(HelloWorld, &HelloWorld::testFloat)
REGISTER_COMPONENT_PROPERTY(HelloWorld, &HelloWorld::boo)

using namespace Simple;

int main()
{
	for (const auto& [hashCode, component] : ComponentRegistry::GetInstance()->myTypeErasureComponents)
	{
		std::cout << component.formattedName << std::endl;

		for (const auto& property : component.properties)
		{
			std::cout << property.customVariableName << std::endl;
		}
	}

	ComponentRegistry::GetInstance()->DestroyInstance();

	return 0;
}