#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include <string>

struct NatdanaiTestComponent
{
	char testCharArray[9] = "TestChar"; //TO-DO(v11.0.1): make this work

	int* testIntPtr = nullptr;

	char testChar = 'A';
	std::string testString = "TestString";
	Math::Vector4f testVec4 = { 1.0f, 2.0f, 3.0f, 4.0f };
	Math::Vector3f testVec3 = { 1.0f, 2.0f, 3.0f };
	Math::Vector2f testVec2 = { 1.0f, 2.0f };
	int testInt = 0;
	float testFloat = 3000.0f;
	bool testBool = false;
};

#include "Engine/ECS/ECSReflection.hpp"

REGISTER_COMPONENT(NatdanaiTestComponent);

EXPOSE_VARIABLE(&NatdanaiTestComponent::testCharArray); //TO-DO(v11.0.1): make register property work with pointers & arrays

EXPOSE_VARIABLE(&NatdanaiTestComponent::testIntPtr);

EXPOSE_VARIABLE(&NatdanaiTestComponent::testChar);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testString);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testBool);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testInt);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testFloat);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testVec2);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testVec3);
EXPOSE_VARIABLE(&NatdanaiTestComponent::testVec4);