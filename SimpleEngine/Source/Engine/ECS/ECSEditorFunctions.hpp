#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"

#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"

#include  <string>

template <size_t N>
constexpr const char* ExtractVariableNameFromDataTypeName(const char(&name)[N])
{
	for (size_t i = N - 1; i > 0; --i)
	{
		if (name[i - 1] == ':')
		{
			return name + i;
		}
	}

	return name;
}

bool ViewAndEditValue(char& aValue, const std::string& aVariableName);
bool ViewAndEditValue(const std::string& aValue, const std::string& aVariableName);

bool ViewAndEditValue(bool& aValue, const std::string& aVariableName);
bool ViewAndEditValue(int& aValue, const std::string& aVariableName);
bool ViewAndEditValue(int*& aValue, const std::string& aVariableName);
bool ViewAndEditValue(float& aValue, const std::string& aVariableName);

bool ViewAndEditValue(Math::Vector2f& aValue, const std::string& aVariableName);
bool ViewAndEditValue(Math::Vector3f& aValue, const std::string& aVariableName);
bool ViewAndEditValue(Math::Vector4f& aValue, const std::string& aVariableName);
bool ViewAndEditValue(Math::Transform& aValue, const std::string& aVariableName);

bool ViewAndEditValue(const Graphics::Mesh*& aValue, const std::string& aVariableName);
bool ViewAndEditValue(const Graphics::Shader*& aValue, const std::string& aVariableName);

bool CustomViewAndEditValue(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName);

template<typename T, size_t N>
bool ViewAndEditValue(std::array<T, N>& aValue, const std::string& aVariableName)
{
	return CustomViewAndEditValue(aValue, aVariableName);
}