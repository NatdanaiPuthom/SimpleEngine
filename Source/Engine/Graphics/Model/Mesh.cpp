#include "stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/Graphics/Texture/Texture.hpp"

Mesh::Mesh()
	: myShader(std::make_shared<Shader>())
	, myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myGraphicsEngine(nullptr)
{
	for (auto& texture : myTextures)
	{
		texture = std::make_unique<Texture>();
	}
}

Mesh::~Mesh()
{
}

const bool Mesh::Init(const MeshData& aMeshData, const char* aPSShaderFile, const char* aVSShaderFile)
{
	myGraphicsEngine = SimplyGlobal::GetGraphicsEngine();
	myMeshData = aMeshData;

	auto device = myGraphicsEngine->GetDevice();

	if (!CreateVertexBuffer(device))
		return false;

	if (!CreateIndexBuffer(device))
		return false;

	if (!CreateObjectBuffer())
		return false;

	if (!myShader->Init(device, aPSShaderFile, aVSShaderFile))
		return false;

	{ //Test
		const bool success = myTextures[0]->Init(device, "Assets/tga/Uppgift6/Grass_c.dds");
		if (success == false)
			return false;

		const bool success2 = myTextures[1]->Init(device, "Assets/tga/Uppgift6/Rock_c.dds");
		if (success2 == false)
			return false;

		const bool success3 = myTextures[2]->Init(device, "Assets/tga/Uppgift6/Snow_c.dds");
		if (success3 == false)
			return false;

		const bool success4 = myTextures[3]->Init(device, "Assets/tga/Uppgift6/Grass_n.dds");
		if (success4 == false)
			return false;

		const bool success5 = myTextures[4]->Init(device, "Assets/tga/Uppgift6/Rock_n.dds");
		if (success5 == false)
			return false;

		const bool success6 = myTextures[5]->Init(device, "Assets/tga/Uppgift6/Snow_n.dds");
		if (success6 == false)
			return false;

		myTextures[0]->Bind(myGraphicsEngine->GetContext(), 0);
		myTextures[1]->Bind(myGraphicsEngine->GetContext(), 1);
		myTextures[2]->Bind(myGraphicsEngine->GetContext(), 2);

		myTextures[3]->Bind(myGraphicsEngine->GetContext(), 3);
		myTextures[4]->Bind(myGraphicsEngine->GetContext(), 4);
		myTextures[5]->Bind(myGraphicsEngine->GetContext(), 5);
	}

	return true;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	const auto context = myGraphicsEngine->GetContext();

	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	myShader->SetShader(context.Get());

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = myModelToWorld;
	myObjectBuffer->Bind(1);
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	context->DrawIndexed(static_cast<UINT>(myMeshData.myIndices.size()), 0, 0);
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

bool Mesh::CreateObjectBuffer()
{
	std::shared_ptr<Camera> camera = myGraphicsEngine->GetCamera();

	ObjectBufferData objectBuffer =
	{
		camera->GetModelToWorldMatrix().GetFastInverse() * camera->GetProjectionMatrix()
	};

	if (!myObjectBuffer->Init(myGraphicsEngine, sizeof(ObjectBufferData), &objectBuffer))
		return false;

	return true;
}

void Mesh::SetPosition(const SimpleUtilities::Vector3f& aPosition)
{
	myModelToWorld.SetPosition(aPosition);
}

int Mesh::GetIndexCount()
{
	return static_cast<int>(myMeshData.myIndices.size());
}

ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return myVertexBuffer;
}

ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return myIndexBuffer;
}

Shader& Mesh::GetShader()
{
	return *myShader;
}

SimpleUtilities::Matrix4x4f& Mesh::GetModelToWorldMatrix()
{
	return myModelToWorld;
}
