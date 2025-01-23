#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Managers/InstancerManager.hpp"
#include "Engine/Global.hpp"

namespace Graphics
{
	InstancerManager::InstancerManager()
	{
	}

	InstancerManager::~InstancerManager()
	{
	}

	void InstancerManager::Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		if (!CreateInstanceBuffer(aDevice))
		{
			assert(false && "Failed to create InstanceBuffer");
		}
	}

	void InstancerManager::UpdateInstanceBuffer(const std::vector<TransformBufferData>& aInstanceData)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = Global::GetGraphicsEngine()->GetContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		const HRESULT hr = context->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hr))
		{
			return;
		}

		memcpy(mappedResource.pData, aInstanceData.data(), sizeof(TransformBufferData) * aInstanceData.size());
		context->Unmap(myInstanceBuffer.Get(), 0);
	}

	const bool InstancerManager::CreateInstanceBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice)
	{
		D3D11_BUFFER_DESC instanceBufferDesc = {};
		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.ByteWidth = sizeof(TransformBufferData) * GLOBAL_INSTANCER_MAX_INSTANCE;
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.MiscFlags = 0;
		instanceBufferDesc.StructureByteStride = 0;

		const HRESULT result = aDevice->CreateBuffer(&instanceBufferDesc, nullptr, &myInstanceBuffer);
		return SUCCEEDED(result);
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> InstancerManager::GetInstanceBuffer()
	{
		return myInstanceBuffer;
	}

	std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash> InstancerManager::SortInstances(const std::vector<MeshInstance>& aMeshInstances)
	{
		std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash> sortedInstances;

		for (const Graphics::MeshInstance& instance : aMeshInstances)
		{
			MeshTextureKey key = std::make_tuple(instance.mesh, instance.albedoTexture, instance.normalTexture, instance.materialTexture);
			sortedInstances[key].push_back(instance);
		}

		return sortedInstances;
	}
}
