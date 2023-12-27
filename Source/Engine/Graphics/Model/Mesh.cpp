#include "Engine/Precomplier/stdafx.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

const bool Mesh::Init(const MeshData& aMeshData)
{
	myMeshData = aMeshData;

	auto device = SimpleGlobal::GetGraphicsEngine()->GetDevice();

	if (!CreateVertexBuffer(device))
		return false;

	if (!CreateIndexBuffer(device))
		return false;

	return true;
}

bool Mesh::CreateVertexBuffer(ComPtr<ID3D11Device> aDevice)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myMeshData.myVertices.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = &myMeshData.myVertices[0];
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
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myMeshData.myIndices.size());
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = &myMeshData.myIndices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result))
		return false;

	return true;
}
