#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSLoadData.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Global.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Model/Skeleton.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/BufferData.hpp"

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

	bool LoadAndSetDataFromJSON(std::string& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		if (aJSONData[aVariableName].is_string())
		{
			aValue = aJSONData[aVariableName];
		}
		else
		{
			aValue = "";
		}

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

	bool LoadAndSetDataFromJSON(Math::Vector2f& aVector, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aVector.x = aJSONData[aVariableName]["x"];
		aVector.y = aJSONData[aVariableName]["y"];

		return true;
	}

	bool LoadAndSetDataFromJSON(Math::Vector3f& aVector, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aVector.x = aJSONData[aVariableName]["x"];
		aVector.y = aJSONData[aVariableName]["y"];
		aVector.z = aJSONData[aVariableName]["z"];

		return true;
	}

	bool LoadAndSetDataFromJSON(Math::Vector4f& aVector, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aVector.x = aJSONData[aVariableName]["x"];
		aVector.y = aJSONData[aVariableName]["y"];
		aVector.z = aJSONData[aVariableName]["z"];
		aVector.w = aJSONData[aVariableName]["w"];

		return true;
	}

	bool LoadAndSetDataFromJSON(Graphics::PointLightData& aPointLight, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aPointLight.color.x = aJSONData[aVariableName]["Color"]["x"];
		aPointLight.color.y = aJSONData[aVariableName]["Color"]["y"];
		aPointLight.color.z = aJSONData[aVariableName]["Color"]["z"];
		aPointLight.color.w = aJSONData[aVariableName]["Intensity"];
		aPointLight.radius = aJSONData[aVariableName]["Radius"];

		return true;
	}

	bool LoadAndSetDataFromJSON(Graphics::Camera& aCamera, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		const Math::Vector2ui resolution = Global::GetResolution();

		Math::Vector3f position;
		position.x = aJSONData[aVariableName]["Position"]["x"];
		position.y = aJSONData[aVariableName]["Position"]["y"];
		position.z = aJSONData[aVariableName]["Position"]["z"];

		aCamera.SetPosition(position);
		aCamera.SetMoveSpeed(aJSONData[aVariableName]["MoveSpeed"]);
		aCamera.SetHorizontalFoV(aJSONData[aVariableName]["HorizontalFoV"], resolution);
		aCamera.SetNearPlane(aJSONData[aVariableName]["NearPlane"], resolution);
		aCamera.SetFarPlane(aJSONData[aVariableName]["FarPlane"], resolution);

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
		aTexture = Global::GetGraphicsEngine()->GetTextureManager()->GetTexture(filePath.c_str()).get();

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
			aShader = graphicsEngine->GetShaderManager()->GetShader(pixelShaderPath.c_str(), vertexShaderPath.c_str()).get();
			return true;
		}

		return false;
	}

	bool LoadAndSetDataFromJSON(const Graphics::Skeleton*& aSkeleton, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		const std::string skeletonRelativeFilePath = aJSONData[aVariableName];

		if (skeletonRelativeFilePath.empty() == true)
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

	bool LoadAndSetDataFromJSON(const Graphics::Animation*& aAnimation, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		if (aJSONData.contains(aVariableName) == false)
		{
			Simple::Console::Print("LoadData: key does not exist", Simple::ConsoleTextColor::Red);
			return false;
		}

		const std::string animationRelativeFilePath = aJSONData[aVariableName];

		if (animationRelativeFilePath.empty() == true)
		{
			return false;
		}

		aAnimation = Global::GetModelFactory()->LoadAnimationFBX(animationRelativeFilePath);
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
					aTextures[i] = graphicsEngine->GetTextureManager()->GetTexture(albedoTexture.c_str()).get();
					success = true;
				}

				break;
			case Graphics::Global_Slot_Normal:
				normalTexture = aJSONData[aVariableName]["Normal"];

				if (normalTexture.empty() == false)
				{
					aTextures[i] = graphicsEngine->GetTextureManager()->GetTexture(normalTexture.c_str()).get();
					success = true;
				}

				break;
			case Graphics::Global_Slot_Material:
				materialTexture = aJSONData[aVariableName]["Material"];

				if (materialTexture.empty() == false)
				{
					aTextures[i] = graphicsEngine->GetTextureManager()->GetTexture(materialTexture.c_str()).get();
					success = true;
				}

				break;
			}
		}

		return success;
	}
}