#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSLoadData.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Engine/Global.hpp"

namespace ECS
{
	bool LoadAndSetDataFromJSON(int& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aValue = aJSONData[aVariableName];
		return true;
	}

	bool LoadAndSetDataFromJSON(float& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aValue = aJSONData[aVariableName];
		return true;
	}

	bool LoadAndSetDataFromJSON(bool& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aValue = aJSONData[aVariableName];
		return true;
	}

	bool LoadAndSetDataFromJSON(Math::Transform& aTransform, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		Math::Vector3f position;
		position.x = aJSONData[aVariableName]["Position"]["x"];
		position.y = aJSONData[aVariableName]["Position"]["y"];
		position.z = aJSONData[aVariableName]["Position"]["z"];

		Math::Vector3f rotation;
		rotation.x = aJSONData[aVariableName]["Rotation"]["x"];
		rotation.y = aJSONData[aVariableName]["Rotation"]["y"];
		rotation.z = aJSONData[aVariableName]["Rotation"]["z"];

		Math::Vector3f scale;
		scale.x = aJSONData[aVariableName]["Scale"]["x"];
		scale.y = aJSONData[aVariableName]["Scale"]["y"];
		scale.z = aJSONData[aVariableName]["Scale"]["z"];

		aTransform.SetPosition(position);
		aTransform.SetRotation(rotation);
		aTransform.SetScale(scale);

		return true;
	}

	bool LoadAndSetDataFromJSON(const Graphics::Mesh*& aMesh, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		const std::string filePath = aJSONData[aVariableName];

		if (filePath.empty() == false)
		{
			aMesh = Global::GetGraphicsEngine()->GetModelFactory()->LoadMesh(filePath);
			return true;
		}

		return false;
	}

	bool LoadAndSetDataFromJSON(const Graphics::Texture*& aTexture, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		const std::string filePath = aJSONData[aVariableName];
		aTexture = Global::GetGraphicsEngine()->GetTexture(filePath.c_str()).get();

		return true;
	}

	bool LoadAndSetDataFromJSON(const Graphics::Shader*& aShader, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		std::string vertexShaderPath;
		std::string pixelShaderPath;

		vertexShaderPath = aJSONData[aVariableName]["VertexShader"];
		pixelShaderPath = aJSONData[aVariableName]["PixelShader"];

		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		if (vertexShaderPath.empty() == false && pixelShaderPath.empty() == false)
		{
			aShader = graphicsEngine->GetShader(pixelShaderPath.c_str(), vertexShaderPath.c_str()).get();
			return true;
		}

		return false;
	}

	bool LoadAndSetDataFromJSON(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		const std::string skeletonRelativeFilePath = aJSONData[aVariableName];

		if (skeletonRelativeFilePath.empty())
		{
			return false;
		}

		const Graphics::Skeleton* skeleton = Global::GetModelFactory()->LoadSkeleton(aJSONData[aVariableName]);

		if (skeleton != nullptr)
		{
			aSkeleton = skeleton;
			return true;
		}

		return true;
	}

	bool CustomLoadAndSetDataFromJSON(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		std::string albedoTexture;
		std::string normalTexture;
		std::string materialTexture;

		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		bool success = false;

		for (size_t i = 0; i < aTextures.size(); ++i)
		{
			switch (i)
			{
			case Graphics::Global_Slot_Albedo:
				albedoTexture = aJSONData[aVariableName]["Albedo"];

				if (albedoTexture.empty() == false)
				{
					aTextures[i] = graphicsEngine->GetTexture(albedoTexture.c_str()).get();
					success = true;
				}

				break;
			case Graphics::Global_Slot_Normal:
				normalTexture = aJSONData[aVariableName]["Normal"];

				if (normalTexture.empty() == false)
				{
					aTextures[i] = graphicsEngine->GetTexture(normalTexture.c_str()).get();
					success = true;
				}

				break;
			case Graphics::Global_Slot_Material:
				materialTexture = aJSONData[aVariableName]["Material"];

				if (materialTexture.empty() == false)
				{
					aTextures[i] = graphicsEngine->GetTexture(materialTexture.c_str()).get();
					success = true;
				}

				break;
			}
		}

		return success;
	}
}