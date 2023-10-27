#include "stdafx.h"
#include "Camera/Camera.h"
#include "Shaders/Shader.h"
#include "ConstantBuffer/ConstantBuffer.h"

Model::Model()
	: myShader(std::make_shared<Shader>())
	, myFrameBuffer(std::make_unique<ConstantBuffer>())
	, myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myTimeBuffer(std::make_unique<ConstantBuffer>())
{
}

Model::~Model()
{
}

bool Model::Init(GraphicsEngine* aGraphicsEngine, const std::vector<Vertex>& aVertices, const std::vector<unsigned int>& aIndices, const SimpleUtilities::Matrix4x4f& aModelToWorld, ID3D11Device* aDevice)
{
	myVertices = aVertices;
	myIndices = aIndices;
	myModelToWorld = aModelToWorld;

	std::shared_ptr<Camera> camera = aGraphicsEngine->GetCamera();

	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myVertices.size());
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myVertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
			return false;
	}

	{
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myIndices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myIndices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		HRESULT result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
			return false;
	}

	{
		FrameBufferData frameBuffer =
		{
			camera->GetModelToWorldMatrix().GetFastInverse() * camera->GetProjectionMatrix()
		};

		if (!myFrameBuffer->Init(aGraphicsEngine, sizeof(FrameBufferData), &frameBuffer))
			return false;
	}

	return true;
}

void Model::Draw(const float aDeltaTime)
{
	aDeltaTime;
}

int Model::GetIndexCount()
{
	return static_cast<int>(myIndices.size());
}

ComPtr<ID3D11Buffer> Model::GetVertexBuffer()
{
	return myVertexBuffer;
}

ComPtr<ID3D11Buffer> Model::GetIndexBuffer()
{
	return myIndexBuffer;
}

Shader& Model::GetShader()
{
	return *myShader;
}

SimpleUtilities::Matrix4x4f& Model::GetModelToWorldMatrix()
{
	return myModelToWorld;
}

std::vector<D3D11_INPUT_ELEMENT_DESC> Model::GetInputLayout()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return inputLayout;
}
