#pragma once

struct ID3D11Buffer;
struct ID3D11Device;

namespace Drawer
{
	class BoundingBoxDrawer;
	class Renderer;
}

struct Vertex final
{
	SimpleUtilities::Vector4f position = { 0,0,0,0 };
	SimpleUtilities::Vector4f color = { 1,1,1,1 };
	SimpleUtilities::Vector3f normal = { 0,0,0 };
	SimpleUtilities::Vector3f tangent = { 0,0,0 };
	SimpleUtilities::Vector3f bitangent = { 0,0,0 };
	SimpleUtilities::Vector2f uv = { 0,0 };
	float clip = 0.0f;
};

struct MeshData final
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

struct BoundingBox final
{
	SimpleUtilities::Vector3f min;
	SimpleUtilities::Vector3f max;
};

namespace Simple
{
	class Mesh final
	{
		friend class Drawer::Renderer;
		friend class Drawer::BoundingBoxDrawer;
	public:
		Mesh();
		~Mesh();

		const bool Init(const MeshData& aMeshData);
		const BoundingBox& GetBoundingBox() const;
	private:
		bool CreateVertexBuffer(ComPtr<ID3D11Device> aDevice);
		bool CreateIndexBuffer(ComPtr<ID3D11Device> aDevice);
		void CreateBoundingBox();
	private:
		MeshData myMeshData;
		BoundingBox myBoundingBox;

		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;
	};
}