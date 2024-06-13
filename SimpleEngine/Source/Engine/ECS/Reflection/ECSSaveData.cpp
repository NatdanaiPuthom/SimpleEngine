#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSSaveData.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"

namespace ECS
{
	nlohmann::json ReturnDataAsJSON(bool& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(int& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(float& aValue, const std::string& aVariableName)
	{
		nlohmann::json json;
		json[aVariableName] = aValue;
		return json;
	}

	nlohmann::json ReturnDataAsJSON(Math::Transform& aTransform, const std::string& aVariableName)
	{
		nlohmann::json json;

		Math::Vector3f position = aTransform.GetPosition();
		Math::Vector3f rotation = aTransform.GetRotation();
		Math::Vector3f scale = aTransform.GetScale();

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
			textureName = aTexture->GetTextureName();
			json[aVariableName] = textureName;
		}

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