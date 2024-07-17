#pragma once
#include "Engine/Math/Transform.hpp"
#include "External/nlohmann/json.hpp"
#include <string>
#include <array>

namespace Graphics
{
	class Mesh;
	class Shader;
	class Texture;
	class Skeleton;

	struct Animation;
}

namespace ECS
{
	nlohmann::json ReturnDataAsJSON(bool& aValue, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(int& aValue, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(float& aValue, const std::string& aVariableName);

	nlohmann::json ReturnDataAsJSON(Math::Transform& aTransform, const std::string& aVariableName);
	nlohmann::json ReturnDataAsJSON(Math::Vector3f& aVector, const std::string& aVariableName);

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