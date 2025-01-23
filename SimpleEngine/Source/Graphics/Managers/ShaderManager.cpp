#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Managers/ShaderManager.hpp"
#include <d3d11.h>

namespace Graphics
{
	ShaderManager::ShaderManager()
		: myDevicePointer(nullptr)
	{
	}

	ShaderManager::~ShaderManager()
	{
	}

	void ShaderManager::Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		myDevicePointer = aDevice;
	}

	const bool ShaderManager::AddShader(const char* aPSFile, const char* aVSFile)
	{
		std::string PSKey(aPSFile);
		std::string VSKey(aVSFile);

		std::shared_ptr<Shader> shader = std::make_shared<Shader>();

		if (!shader->Init(myDevicePointer.Get(), PSKey, VSKey))
			return false;

		myLoadedShaders.emplace(std::make_pair(PSKey, VSKey), shader);

		return true;
	}

	std::shared_ptr<const Shader> ShaderManager::GetShader(const char* aPSFile, const char* aVSFile)
	{
		auto shader = myLoadedShaders.find({ aPSFile, aVSFile });

		if (shader != myLoadedShaders.end())
		{
			return shader->second;
		}
		else if (shader == myLoadedShaders.end())
		{
			const bool success = AddShader(aPSFile, aVSFile);
			if (success == false)
				assert(false && "Unable to create Shader");
			else
			{
				shader = myLoadedShaders.find({ aPSFile, aVSFile });
				return shader->second;
			}
		}

		return nullptr;
	}

	std::shared_ptr<const Shader> ShaderManager::GetShader(const eShaderType aShaderType)
	{
		std::shared_ptr<const Shader> shader = nullptr;

		switch (aShaderType)
		{
		case eShaderType::Unlit_Default:
			shader = GetShader("Shaders\\DefaultPS.cso", "Shaders\\DefaultVS.cso");
			break;
		case eShaderType::PBR_Default:
			shader = GetShader("Shaders\\GBufferPS.cso", "Shaders\\DefaultVS.cso");
			break;
		case eShaderType::Instanced_Unlit_Default:
			shader = GetShader("Shaders\\DefaultPS.cso", "Shaders\\InstancedDefaultVS.cso");
			break;
		case eShaderType::Animated:
			shader = GetShader("Shaders\\GBufferPS.cso", "Shaders\\AnimatedModelVS.cso");
			break;
		case eShaderType::SkyBox:
			shader = GetShader("Shaders\\SkyBoxPS.cso", "Shaders\\DefaultVS.cso");
			break;
		case eShaderType::Deferred:
			shader = GetShader("Shaders\\DeferredPS.cso", "Shaders\\FullScreenVS.cso");
			break;
		case eShaderType::PointLight:
			shader = GetShader("Shaders\\PointLightCullPS.cso", "Shaders\\DefaultVS.cso");
			break;
		case eShaderType::PostProcessing:
			shader = GetShader("Shaders\\PostProcessingPS.cso", "Shaders\\FullScreenVS.cso");
			break;
		case eShaderType::GaussianBlur:
			shader = GetShader("Shaders\\GaussianBlurPS.cso", "Shaders\\FullScreenVS.cso");
			break;
		case eShaderType::Bloom:
			shader = GetShader("Shaders\\BloomPS.cso", "Shaders\\FullScreenVS.cso");
			break;
		case eShaderType::BloomPixelFilter:
			shader = GetShader("Shaders\\BloomPixelFilterPS.cso", "Shaders\\FullScreenVS.cso");
			break;
		case eShaderType::Copy:
			shader = GetShader("Shaders\\FullScreenCopyPS.cso", "Shaders\\FullScreenVS.cso");
			break;
		default:
			break;
		}

		return shader;
	}
}
