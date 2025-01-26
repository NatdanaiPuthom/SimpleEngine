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
	class ClassInstanceProxy;
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
	struct ViewAndEditResult final
	{
		bool myIsActive = false;
		bool myIsEdited = false;

		void operator |=(const ViewAndEditResult aOther)
		{
			myIsActive |= aOther.myIsActive;
			myIsEdited |= aOther.myIsEdited;
		}
	};
}

namespace ECS
{
	ViewAndEditResult ViewAndEditValue(char& aValue, const std::string& aVariableName);

	ViewAndEditResult ViewAndEditValue(bool& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(int& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(int*& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(float& aValue, const std::string& aVariableName);

	ViewAndEditResult ViewAndEditValue(std::string& aValue, const std::string& aVariableName);

	ViewAndEditResult ViewAndEditValue(Math::Vector2f& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(Math::Vector3f& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(Math::Vector4f& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(Math::Transform& aValue, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(Graphics::PointLightData& aPointLightData, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(Graphics::Camera& aCamera, const std::string& aVariableName);

	ViewAndEditResult ViewAndEditValue(const Graphics::Mesh*& aMesh, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(const Graphics::Shader*& aShader, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(const Graphics::Texture*& aTexture, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName);
	ViewAndEditResult ViewAndEditValue(const Graphics::Animation*& aAnimation, const std::string& aVariableName);

	ViewAndEditResult ViewAndEditValue(Fly::ClassInstanceProxy& aClassInstanceView, const std::string& aVariableName);

	ViewAndEditResult CustomViewAndEditValue(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName);

	template<typename T, size_t N>
	ViewAndEditResult ViewAndEditValue(std::array<T, N>& aValue, const std::string& aVariableName)
	{
		return CustomViewAndEditValue(aValue, aVariableName);
	}
}