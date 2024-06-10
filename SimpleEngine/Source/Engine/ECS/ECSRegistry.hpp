#pragma once
#include "Engine/ECS/ECSReflection.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"
#include <string>

REGISTER_DATA_TYPE(float);
REGISTER_DATA_TYPE(int);
REGISTER_DATA_TYPE(bool);
REGISTER_DATA_TYPE(char);

namespace std
{
	REGISTER_DATA_TYPE(string);
}

namespace Math
{
	REGISTER_DATA_TYPE(Vector2f);
	REGISTER_DATA_TYPE(Vector3f);
	REGISTER_DATA_TYPE(Vector4f);
	REGISTER_DATA_TYPE(Transform);
}