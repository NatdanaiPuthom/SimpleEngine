#pragma once
#include "Graphics/Model/Mesh.hpp"
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

//
//template<typename T, size_t N>
//bool ViewAndEditValue(std::array<T, N>& aValue, const std::string& aVariableName)
//{
//	for (size_t i = 0; i < aValue.size(); ++i)
//	{
//		aValue[i] = new int(static_cast<int>(i));
//	}
//
//	for (size_t i = 0; i < aValue.size(); ++i)
//	{
//		CustomViewAndEditValue(aValue[i], aVariableName);
//	}
//
//	aValue; aVariableName;
//	return true;
//}