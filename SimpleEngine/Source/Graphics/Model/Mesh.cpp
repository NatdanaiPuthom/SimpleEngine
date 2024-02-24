#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Engine/Global.hpp"

namespace Simple
{
	Mesh::Mesh()
	{
	}

	Mesh::~Mesh()
	{
	}

	const bool Mesh::Init(const MeshData& aMeshData)
	{
		myMeshData = aMeshData;

		CreateBoundingBox();

		auto device = Global::GetGraphicsEngine()->GetDevice();

		if (!CreateVertexBuffer(device))
			return false;

		if (!CreateIndexBuffer(device))
			return false;

		return true;
	}

	const BoundingBox3D& Mesh::GetBoundingBox() const
	{
		return myBoundingBox;
	}

	bool Mesh::CreateVertexBuffer(ComPtr<ID3D11Device> aDevice)
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myMeshData.vertices.size());
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myMeshData.vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
			return false;

		return true;
	}

	bool Mesh::CreateIndexBuffer(ComPtr<ID3D11Device> aDevice)
	{
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myMeshData.indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myMeshData.indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		HRESULT result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
			return false;

		return true;
	}

	void Mesh::CreateBoundingBox()
	{
		BoundingBox3D boundingBox;

		if (myMeshData.vertices.empty())
			assert(false && "MeshData has no vertices");

		boundingBox.min = myMeshData.vertices[0].position.AsVector3();
		boundingBox.max = myMeshData.vertices[0].position.AsVector3();

		for (const auto& vertex : myMeshData.vertices)
		{
			boundingBox.min.x = Math::GetMin(boundingBox.min.x, vertex.position.x);
			boundingBox.min.y = Math::GetMin(boundingBox.min.y, vertex.position.y);
			boundingBox.min.z = Math::GetMin(boundingBox.min.z, vertex.position.z);

			boundingBox.max.x = Math::GetMax(boundingBox.max.x, vertex.position.x);
			boundingBox.max.y = Math::GetMax(boundingBox.max.y, vertex.position.y);
			boundingBox.max.z = Math::GetMax(boundingBox.max.z, vertex.position.z);
		}

		myBoundingBox = boundingBox;
	}
}