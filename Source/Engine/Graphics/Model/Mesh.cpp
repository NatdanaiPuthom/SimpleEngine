#include "stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/Graphics/Texture/Texture.hpp"

Mesh::Mesh()
	: myShader(std::make_shared<Shader>())
	, myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myGraphicsEngine(nullptr)
{
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

	if (!AddTexture(0, "Assets/Textures/DefaultTexture.dds")) //Default texture
		return false;

	return true;
}

const bool Mesh::AddTexture(const int aSlot, const char* aFilePath)
{
	auto device = myGraphicsEngine->GetDevice();

	myTextures[aSlot].reset();
	myTextures[aSlot] = std::make_unique<Texture>();

	if (!myTextures[aSlot]->LoadDDS(aFilePath))
		return false;

	myTextures[aSlot]->Bind(myGraphicsEngine->GetContext(), aSlot);

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
	objectBuffer.modelToWorldMatrix = myTransform.GetMatrix();
	myObjectBuffer->Bind(1);
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	for (unsigned int i = 0; i < myTextures.size(); ++i) //TO-DO: Re-structure how textures are binded so it doesn't need to check for every mesh for every textures
	{
		if (myTextures[i] != nullptr)
			myTextures[i]->Bind(myGraphicsEngine->GetContext(), i);
	}

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
	myTransform.SetPosition(aPosition);
}

void Mesh::SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree)
{
	myTransform.SetRotation(aRotationInDegree);
}

SimpleUtilities::Vector3f Mesh::GetPosition() const
{
	return myTransform.GetPosition();
}

SimpleUtilities::Vector3f Mesh::GetRotation() const
{
	return myTransform.GetRotation();
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

