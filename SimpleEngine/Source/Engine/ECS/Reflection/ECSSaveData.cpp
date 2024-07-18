#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSSaveData.hpp"
#include "Engine/Math/Transform.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"

namespace ECS
{
	nlohmann::json ReturnDataAsJSON(const bool& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(const int& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(const float& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(const std::string& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Math::Transform& aTransform, const std::string& aVariableName)
	{
		nlohmann::json json;

		const Math::Vector3f position = aTransform.GetPosition();
		const Math::Vector3f rotation = aTransform.GetRotation();
		const Math::Vector3f scale = aTransform.GetScale();

		json[aVariableName]["Position"]["x"] = position.x;
		json[aVariableName]["Position"]["y"] = position.y;
		json[aVariableName]["Position"]["z"] = position.z;

		json[aVariableName]["Rotation"]["x"] = rotation.x;
		json[aVariableName]["Rotation"]["y"] = rotation.y;
		json[aVariableName]["Rotation"]["z"] = rotation.z;

		json[aVariableName]["Scale"]["x"] = scale.x;
		json[aVariableName]["Scale"]["y"] = scale.y;
		json[aVariableName]["Scale"]["z"] = scale.z;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Math::Vector2f& aVector, const std::string& aVariableName)
	{
		nlohmann::json json;

		json[aVariableName]["x"] = aVector.x;
		json[aVariableName]["y"] = aVector.y;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Math::Vector3f& aVector, const std::string& aVariableName)
	{
		nlohmann::json json;

		json[aVariableName]["x"] = aVector.x;
		json[aVariableName]["y"] = aVector.y;
		json[aVariableName]["z"] = aVector.z;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Math::Vector4f& aVector, const std::string& aVariableName)
	{
		nlohmann::json json;

		json[aVariableName]["x"] = aVector.x;
		json[aVariableName]["y"] = aVector.y;
		json[aVariableName]["z"] = aVector.z;
		json[aVariableName]["w"] = aVector.w;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Graphics::PointLightData& aPointlight, const std::string& aVariableName)
	{
		nlohmann::json json;

		json[aVariableName]["Color"]["x"] = aPointlight.color.x;
		json[aVariableName]["Color"]["y"] = aPointlight.color.y;
		json[aVariableName]["Color"]["z"] = aPointlight.color.z;
		json[aVariableName]["Intensity"] = aPointlight.color.w;
		json[aVariableName]["Radius"] = aPointlight.radius;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Graphics::Mesh*& aMesh, const std::string& aVariableName)
	{
		nlohmann::json json;
		std::string meshPath;

		if (aMesh != nullptr)
		{
			meshPath = aMesh->GetRelativePath();
		}

		json[aVariableName] = meshPath;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Graphics::Shader*& aShader, const std::string& aVariableName)
	{
		nlohmann::json json;

		std::string vertexShader;
		std::string pixelShader;

		if (aShader != nullptr)
		{
			vertexShader = aShader->GetVertexShaderRelativePath();
			pixelShader = aShader->GetPixelShaderRelativePath();
		}

		json[aVariableName]["VertexShader"] = vertexShader;
		json[aVariableName]["PixelShader"] = pixelShader;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Graphics::Texture*& aTexture, const std::string& aVariableName)
	{
		nlohmann::json json;

		std::string textureName;

		if (aTexture != nullptr)
		{
			textureName = aTexture->GetRelativePath();
		}

		json[aVariableName] = textureName;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName)
	{
		nlohmann::json json;

		std::string skeletonRelativePath;

		if (aSkeleton != nullptr)
		{
			skeletonRelativePath = aSkeleton->GetRelativePath();
		}

		json[aVariableName] = skeletonRelativePath;

		return json;
	}

	nlohmann::json ReturnDataAsJSON(const Graphics::Animation*& aAnimation, const std::string& aVariableName)
	{
		nlohmann::json json;

		std::string animationRelativePath;

		if (aAnimation != nullptr)
		{
			animationRelativePath = aAnimation->relativePath;
		}

		json[aVariableName] = animationRelativePath;

		return json;
	}

	nlohmann::json CustomReturnDataAsJSON(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName)
	{
		nlohmann::json json;

		std::string albedoTexture;
		std::string normalTexture;
		std::string materialTexture;

		for (size_t i = 0; i < aTextures.size(); ++i)
		{
			if (aTextures[i] != nullptr)
			{
				switch (i)
				{
				case Graphics::Global_Slot_Albedo:
					albedoTexture = aTextures[i]->GetRelativePath();
					break;
				case Graphics::Global_Slot_Normal:
					normalTexture = aTextures[i]->GetRelativePath();
					break;
				case Graphics::Global_Slot_Material:
					materialTexture = aTextures[i]->GetRelativePath();
					break;
				}
			}
		}

		json[aVariableName]["Albedo"] = albedoTexture;
		json[aVariableName]["Normal"] = normalTexture;
		json[aVariableName]["Material"] = materialTexture;

		return json;
	}
}