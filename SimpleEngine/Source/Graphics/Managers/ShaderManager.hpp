#pragma once
#include "Engine/SimpleUtilities/HashStuff.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/GraphicsDeclarations.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <wrl/client.h>

struct ID3D11Device;

namespace Graphics
{
	class ShaderManager final
	{
	public:
		ShaderManager();
		~ShaderManager();

		void Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	public:
		const bool AddShader(const char* aPSFile, const char* aVSFile);
	public:
		//NOTE(v9.36.1): Will Add and cache the shader if it does not already exist
		std::shared_ptr<const Shader> GetShader(const char* aPSFile, const char* aVSFile);
		std::shared_ptr<const Shader> GetShader(const eShaderType aShaderType);
	private:
		std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<const Shader>, SimpleUtilities::PairHash, SimpleUtilities::PairEqual> myLoadedShaders;
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> myDevicePointer;
	};
}