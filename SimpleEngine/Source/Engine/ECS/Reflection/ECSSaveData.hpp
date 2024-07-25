#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "External/nlohmann/json.hpp"
#include <string>
#include <array>

namespace Math
{
	class Transform;
}

namespace Graphics
{
	class Mesh;
	class Shader;
	class Texture;
	class Skeleton;
	class Camera;

	struct Animation;
	struct PointLightData;
}

namespace ECS
{
	nlohmann::json ReturnDataAsJSON(const bool& aValue, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const int& aValue, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const float& aValue, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const std::string& aValue, const std::string& aVariableName);

	nlohmann::json ReturnDataAsJSON(const Math::Transform& aTransform, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Math::Vector2f& aVector, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Math::Vector3f& aVector, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Math::Vector4f& aVector, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Graphics::PointLightData& aPointlightData, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Graphics::Camera& aCamera, const std::string& aVariableName);

	nlohmann::json ReturnDataAsJSON(const Graphics::Mesh*& aMesh, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Graphics::Shader*& aShader, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Graphics::Texture*& aTextures, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(const Graphics::Animation*& aAnimation, const std::string& aVariableName);

	nlohmann::json CustomReturnDataAsJSON(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName);

	template<typename T, size_t N>
	nlohmann::json ReturnDataAsJSON(std::array<T, N>& aValue, const std::string& aVariableName)
	{
		return CustomReturnDataAsJSON(aValue, aVariableName);
	}
}