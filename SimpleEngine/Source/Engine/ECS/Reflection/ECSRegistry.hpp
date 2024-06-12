#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"

#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"

#include <string>
#include <array>

#include "Engine/ECS/Reflection/ECSReflection.hpp"

REGISTER_DATATYPE(float);
REGISTER_DATATYPE(int);
REGISTER_DATATYPE(bool);
REGISTER_DATATYPE(char);

namespace std
{
	REGISTER_DATATYPE(string);
	REGISTER_DATATYPE_ARRAY_CONST_POINTER(array, Graphics::Texture, 3); //TO-DO(v11.0.2): figure out a way to not having to have hardcoded "3"
}

namespace Math
{
	REGISTER_DATATYPE(Vector2f);
	REGISTER_DATATYPE(Vector3f);
	REGISTER_DATATYPE(Vector4f);
	REGISTER_DATATYPE(Transform);
}

namespace Graphics
{
	REGISTER_DATATYPE_CONST_POINTER(Mesh);
	REGISTER_DATATYPE_CONST_POINTER(Shader);
	REGISTER_DATATYPE_CONST_POINTER(Texture);
}