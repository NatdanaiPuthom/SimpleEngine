#pragma once
#include "Graphics/GraphicsDeclarations.hpp"
#include <wrl/client.h>
#include <d3d11.h>

struct ID3D11Buffer;

namespace Graphics
{
	class InstancerManager final
	{
	public:
	public:
		InstancerManager();
		~InstancerManager();

		void Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);

		std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash> SortInstances(const std::vector<MeshInstance>& aMeshInstances);
		void UpdateInstanceBuffer(const std::vector<TransformBufferData>& aInstanceData);
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetInstanceBuffer();
	private:
		const bool CreateInstanceBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> myInstanceBuffer;
	};
}