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
	bool LoadAndSetDataFromJSON(int& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(float& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(bool& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData);

	bool LoadAndSetDataFromJSON(std::string& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData);

	bool LoadAndSetDataFromJSON(Math::Transform& aTransform, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(Math::Vector2f& aVector, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(Math::Vector3f& aVector, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(Math::Vector4f& aVector, const std::string& aVariableName, const nlohmann::json& aJSONData);

	bool LoadAndSetDataFromJSON(Graphics::PointLightData& aPointLight, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(Graphics::Camera& aCamera, const std::string& aVariableName, const nlohmann::json& aJSONData);

	bool LoadAndSetDataFromJSON(const Graphics::Mesh*& aMesh, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(const Graphics::Texture*& aTexture, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(const Graphics::Shader*& aShader, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName, const nlohmann::json& aJSONData);
	bool LoadAndSetDataFromJSON(const Graphics::Animation*& aAnimation, const std::string& aVariableName, const nlohmann::json& aJSONData);

	bool CustomLoadAndSetDataFromJSON(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName, const nlohmann::json& aJSONData);

	template<typename T, size_t N>
	bool LoadAndSetDataFromJSON(std::array<T, N>& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		return CustomLoadAndSetDataFromJSON(aValue, aVariableName, aJSONData);
	}
}