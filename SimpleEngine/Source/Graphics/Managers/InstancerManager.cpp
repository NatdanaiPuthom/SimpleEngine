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

	void InstancerManager::SortInstancesByMeshAndTexture(const std::vector<MeshInstance>& aInstances, std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash>& aSortedInstances)
	{
		for (const auto& instance : aInstances)
		{
			MeshTextureKey key = std::make_tuple(instance.mesh, instance.albedoTexture, instance.normalTexture, instance.materialTexture);
			aSortedInstances[key].push_back(instance);
		}
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> InstancerManager::GetInstanceBuffer()
	{
		return myInstanceBuffer;
	}

	void InstancerManager::GenerateInstanceDataWithTextures(std::vector<MeshInstance>& aInstanceData, const int aType, const Math::Vector3f& aOffset)
	{
		auto graphicsEngine = Global::GetGraphicsEngine();
		auto context = graphicsEngine->GetContext();

		std::shared_ptr<const Graphics::Texture> albedo;
		std::shared_ptr<const Graphics::Texture> normal;
		std::shared_ptr<const Graphics::Texture> material;

		switch (aType)
		{
		case 1:
			albedo = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Albedo);
			normal = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Normal);
			material = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Material);
			break;
		case 2:
			albedo = graphicsEngine->GetTextureManager()->GetTexture("Assets\\Textures\\T_Hamster_C.dds");
			normal = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Normal);
			material = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Material);
			break;
		case 3:
			albedo = graphicsEngine->GetTextureManager()->GetTexture("Assets\\Textures\\T_CatScared_C.dds");
			normal = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Normal);
			material = graphicsEngine->GetTextureManager()->GetTexture(Graphics::eTextureType::Default_Material);
			break;
		default:
			break;
		}

		const Graphics::Mesh* mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);

		const int instanceCount = 100;
		const int modelsPerLine = 5;
		const float spacing = 2.0f;
		const float lineSpacing = 2.0f;

		for (int i = 0; i < instanceCount; ++i)
		{
			MeshInstance data;
			data.mesh = mesh;
			data.albedoTexture = albedo.get();
			data.normalTexture = normal.get();
			data.materialTexture = material.get();

			float x = (i % modelsPerLine) * spacing - (modelsPerLine / 2.0f) * spacing;
			float z = static_cast<float>((i / modelsPerLine)) * lineSpacing;
			data.transform.SetPosition(Math::Vector3f(x + aOffset.x * spacing, 0.0f + aOffset.y * spacing, z + aOffset.z * spacing));
			aInstanceData.push_back(data);
		}
	}

	std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash> InstancerManager::CreateTestInstanceData()
	{
		std::vector<MeshInstance> test1;
		std::vector<MeshInstance> test2;
		std::vector<MeshInstance> test3;

		GenerateInstanceDataWithTextures(test1, 1, Math::Vector3f(0, 0, 0));
		GenerateInstanceDataWithTextures(test2, 2, Math::Vector3f(-6, 0, 0));
		GenerateInstanceDataWithTextures(test3, 3, Math::Vector3f(6, 0, 0));

		std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash> sortedInstances;

		SortInstancesByMeshAndTexture(test1, sortedInstances);
		SortInstancesByMeshAndTexture(test2, sortedInstances);
		SortInstancesByMeshAndTexture(test3, sortedInstances);

		return sortedInstances;
	}
}
