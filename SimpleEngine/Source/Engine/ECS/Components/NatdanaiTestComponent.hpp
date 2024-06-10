#pragma once
#include "Engine/ECS/ECSReflection.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include <string>

struct NatdanaiStruct
{
	char testCharArray[9] = "TestChar"; //TO-DO(v11.0.1): make this work

	char testChar = 'A';
	std::string testString = "TestString";
	Math::Vector4f testVec4 = { 1.0f, 2.0f, 3.0f, 4.0f };
	Math::Vector3f testVec3 = { 1.0f, 2.0f, 3.0f };
	Math::Vector2f testVec2 = { 1.0f, 2.0f };
	int testInt = 0;
	float testFloat = 3000.0f;
	bool testBool = false;
};

REGISTER_COMPONENT(NatdanaiStruct);

REGISTER_PROPERTY(&NatdanaiStruct::testCharArray); //TO-DO(v11.0.1): make register property work with pointers & arrays

REGISTER_PROPERTY(&NatdanaiStruct::testChar);
REGISTER_PROPERTY(&NatdanaiStruct::testString);
REGISTER_PROPERTY(&NatdanaiStruct::testBool);
REGISTER_PROPERTY(&NatdanaiStruct::testInt);
REGISTER_PROPERTY(&NatdanaiStruct::testFloat);
REGISTER_PROPERTY(&NatdanaiStruct::testVec2);
REGISTER_PROPERTY(&NatdanaiStruct::testVec3);
REGISTER_PROPERTY(&NatdanaiStruct::testVec4);