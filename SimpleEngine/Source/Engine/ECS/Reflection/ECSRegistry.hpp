#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"

#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/BufferData.hpp"

#include <string>
#include <array>

#include "Engine/ECS/Reflection/ECSReflection.hpp"

inline ECS::__RegisterDataType<float> Global_ECS_Registered_Datatype_float;
inline ECS::__RegisterDataType<int> Global_ECS_Registered_Datatype_int;;
inline ECS::__RegisterDataType<bool> Global_ECS_Registered_Datatype_bool;;
inline ECS::__RegisterDataType<char> Global_ECS_Registered_Datatype_char;;

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
	REGISTER_DATATYPE(PointLightData);
	REGISTER_DATATYPE_CONST_POINTER(Mesh);
	REGISTER_DATATYPE_CONST_POINTER(Shader);
	REGISTER_DATATYPE_CONST_POINTER(Texture);
	REGISTER_DATATYPE_CONST_POINTER(Skeleton);
	REGISTER_DATATYPE_CONST_POINTER(Animation);
}