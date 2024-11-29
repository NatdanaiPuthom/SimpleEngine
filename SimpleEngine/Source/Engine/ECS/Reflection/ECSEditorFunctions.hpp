#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"

#include <string>
#include <array>

namespace Graphics
{
	class Mesh;
	class Shader;
	class Texture;
	class Skeleton;
	class Camera;

	struct PointLightData;
	struct Animation;
}

namespace Fly
{
	class ClassInstanceFacade;
}

namespace ECS
{
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
}

namespace ECS
{
	bool ViewAndEditValue(char& aValue, const std::string& aVariableName);

	bool ViewAndEditValue(bool& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(int& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(int*& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(float& aValue, const std::string& aVariableName);

	bool ViewAndEditValue(std::string& aValue, const std::string& aVariableName);

	bool ViewAndEditValue(Math::Vector2f& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(Math::Vector3f& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(Math::Vector4f& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(Math::Transform& aValue, const std::string& aVariableName);
	bool ViewAndEditValue(Graphics::PointLightData& aPointLightData, const std::string& aVariableName);
	bool ViewAndEditValue(Graphics::Camera& aCamera, const std::string& aVariableName);

	bool ViewAndEditValue(const Graphics::Mesh*& aMesh, const std::string& aVariableName);
	bool ViewAndEditValue(const Graphics::Shader*& aShader, const std::string& aVariableName);
	bool ViewAndEditValue(const Graphics::Texture*& aTexture, const std::string& aVariableName);
	bool ViewAndEditValue(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName);
	bool ViewAndEditValue(const Graphics::Animation*& aAnimation, const std::string& aVariableName);

	bool ViewAndEditValue(Fly::ClassInstanceFacade& aClassInstanceView, const std::string& aVariableName);

	bool CustomViewAndEditValue(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName);

	template<typename T, size_t N>
	bool ViewAndEditValue(std::array<T, N>& aValue, const std::string& aVariableName)
	{
		return CustomViewAndEditValue(aValue, aVariableName);
	}
}