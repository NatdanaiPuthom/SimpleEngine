#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"
#include <string>

#include "Engine/ECS/ECSReflection.hpp"

REGISTER_DATATYPE(float);
REGISTER_DATATYPE(int);
REGISTER_DATATYPE(bool);
REGISTER_DATATYPE(char);

REGISTER_DATATYPE_POINTER(int);

namespace std
{
	REGISTER_DATATYPE(string);
}

namespace Math
{
	REGISTER_DATATYPE(Vector2f);
	REGISTER_DATATYPE(Vector3f);
	REGISTER_DATATYPE(Vector4f);
	REGISTER_DATATYPE(Transform);
}