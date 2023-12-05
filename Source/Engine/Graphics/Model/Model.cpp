#include "stdafx.h"
#include "Engine/global.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Engine/Graphics/Texture/Texture.h"

Model::Model()
	: myShader(std::make_shared<Shader>())
	, myFrameBuffer(std::make_unique<ConstantBuffer>())
	, myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myTimeBuffer(std::make_unique<ConstantBuffer>())
	, myTexture(std::make_shared<Texture>())
	, myGraphicsEngine(nullptr)
{
}

Model::~Model()
{
}

const bool Model::Init(GraphicsEngine* aGraphicsEngine, const std::vector<Vertex>& aVertices, const std::vector<unsigned int>& aIndices, const SimpleUtilities::Matrix4x4f& aModelToWorld, eShaderType aShaderType)
{
	myGraphicsEngine = aGraphicsEngine;

	myVertices = aVertices;
	myIndices = aIndices;
	myModelToWorld = aModelToWorld;

	std::shared_ptr<Camera> camera = myGraphicsEngine->GetCamera();
	auto& device = myGraphicsEngine->GetDevice();

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

		HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
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
		HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
			return false;
	}

	{
		FrameBufferData frameBuffer =
		{
			camera->GetModelToWorldMatrix().GetFastInverse() * camera->GetProjectionMatrix()
		};

		if (!myFrameBuffer->Init(myGraphicsEngine, sizeof(FrameBufferData), &frameBuffer))
			return false;
	}

	{
		ObjectBufferData objectBuffer =
		{
			camera->GetModelToWorldMatrix().GetFastInverse() * camera->GetProjectionMatrix()
		};

		if (!myObjectBuffer->Init(myGraphicsEngine, sizeof(ObjectBufferData), &objectBuffer))
			return false;
	}

	{
		TimeBufferData timeBuffer =
		{
			1.0f
		};

		if (!myTimeBuffer->Init(aGraphicsEngine, sizeof(TimeBufferData), &timeBuffer))
			return false;
	}

	const char* shaderType = "DefaultVS.cso";
	switch (aShaderType)
	{
	case eShaderType::Default:
		shaderType = "DefaultVS.cso";
		break;
	case eShaderType::Colorful:
		shaderType = "DefaultColorfulVS.cso";
		break;
	default:
		shaderType = "DefaultVS.cso";
		break;
	}

	if (!myShader->Init(device, "DefaultPS.cso", shaderType))
		return false;

	{ //Test No Clue What It Is
		const int channels = 4;
		unsigned char* aRGBAPixels = new unsigned char[1280 * 720 * channels];

		for (int i = 0; i < 1280 * 720; ++i) {
			aRGBAPixels[i * channels] = 255;   // Red component
			aRGBAPixels[i * channels + 1] = 0; // Green component
			aRGBAPixels[i * channels + 2] = 0; // Blue component
			aRGBAPixels[i * channels + 3] = 255; // Alpha component
		}

		const bool success = myTexture->Init(device, aRGBAPixels, 1280, 720);
		delete[] aRGBAPixels;

		if (success == false)
			return false;
	}

	return true;
}

void Model::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	auto& context = myGraphicsEngine->GetContext();
	auto camera = myGraphicsEngine->GetCamera();

	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	myShader->SetShader(context.Get());

	FrameBufferData frameBuffer = {};
	frameBuffer.worldToClipMatrix = SimpleUtilities::Matrix4x4f::GetFastInverse(camera->GetModelToWorldMatrix()) * camera->GetProjectionMatrix();
	myFrameBuffer->Bind(0);
	myFrameBuffer->Update(sizeof(FrameBufferData), &frameBuffer);

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = myModelToWorld;
	myObjectBuffer->Bind(1);
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	TimeBufferData timeBuffer = {};
	timeBuffer.time = static_cast<float>(SimplyGlobal::GetTotalTime());
	myTimeBuffer->Bind(2);
	myTimeBuffer->Update(sizeof(TimeBufferData), &timeBuffer);

	context->DrawIndexed(static_cast<UINT>(myIndices.size()), 0, 0);
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
