#pragma once
#include "Engine/SimpleUtilities/Bounds.hpp"

struct ID3D11Buffer;
struct ID3D11Device;

namespace Drawer
{
	class BoundingBoxDrawer;
	class Renderer;
}

namespace Simple
{
	struct Vertex final
	{
		Math::Vector4f position = { 0,0,0,0 };
		Math::Vector4f color = { 1,1,1,1 };
		Math::Vector4f bones = { 0,0,0,0 };
		Math::Vector4f weights = { 0,0,0,0 };
		Math::Vector3f normal = { 0,0,0 };
		Math::Vector3f tangent = { 0,0,0 };
		Math::Vector3f bitangent = { 0,0,0 };
		Math::Vector2f uv = { 0,0 };
		float clip = 0.0f;
	};

	struct MeshData final
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	class Mesh final
	{
		friend class Drawer::Renderer;
		friend class Drawer::BoundingBoxDrawer;
	public:
		Mesh();
		~Mesh();

		const bool Init(const MeshData& aMeshData);
		const BoundingBox3D& GetBoundingBox() const;
	private:
		bool CreateVertexBuffer(ComPtr<ID3D11Device> aDevice);
		bool CreateIndexBuffer(ComPtr<ID3D11Device> aDevice);
		void CreateBoundingBox();
	private:
		MeshData myMeshData;
		BoundingBox3D myBoundingBox;

		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;
	};
}