#pragma once
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
bool ViewAndEditValue(float& aValue, const std::string& aVariableName);

bool ViewAndEditValue(Math::Vector2f& aValue, const std::string& aVariableName);
bool ViewAndEditValue(Math::Vector3f& aValue, const std::string& aVariableName);
bool ViewAndEditValue(Math::Vector4f& aValue, const std::string& aVariableName);
bool ViewAndEditValue(Math::Transform& aValue, const std::string& aVariableName);