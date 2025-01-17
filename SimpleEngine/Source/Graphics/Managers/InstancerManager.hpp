#pragma once
#include "Engine/Math/Transform.hpp"
#include <unordered_map>
#include <tuple>
#include <wrl/client.h>
#include <d3d11.h>

struct ID3D11Buffer;

namespace Graphics
{
	class Mesh;
	class Texture;
}

namespace Graphics
{
	constexpr size_t GLOBAL_INSTANCER_MAX_INSTANCE = 1024;

	using MeshTextureKey = std::tuple<const Graphics::Mesh*, const Graphics::Texture*, const Graphics::Texture*, const Graphics::Texture*>;

	struct MeshInstance
	{
		Math::Transform transform;
		const Graphics::Mesh* mesh = nullptr;
		const Graphics::Texture* albedoTexture = nullptr;
		const Graphics::Texture* normalTexture = nullptr;
		const Graphics::Texture* materialTexture = nullptr;
	};

	struct MeshTextureKeyHash
	{
		std::size_t operator()(const MeshTextureKey& key) const
		{
			return 
				std::hash<const Graphics::Mesh*>()(std::get<0>(key)) ^
				std::hash<const Graphics::Texture*>()(std::get<1>(key)) ^
				std::hash<const Graphics::Texture*>()(std::get<2>(key)) ^
				std::hash<const Graphics::Texture*>()(std::get<3>(key));
		}
	};

	class InstancerManager final
	{
	public: //NOTE(v11.4.5): test stuff
		std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash> CreateTestInstanceData();
		void GenerateInstanceDataWithTextures(std::vector<MeshInstance>& aInstanceData, const int aType, const Math::Vector3f& aOffset);
		void SortInstancesByMeshAndTexture(const std::vector<MeshInstance>& aInstances, std::unordered_map<MeshTextureKey, std::vector<MeshInstance>, MeshTextureKeyHash>& aSortedInstances);
	public:
		InstancerManager();
		~InstancerManager();

		void Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);

		void UpdateInstanceBuffer(const std::vector<TransformBufferData>& aInstanceData);
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetInstanceBuffer();
	private:
		const bool CreateInstanceBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> myInstanceBuffer;
	};
}