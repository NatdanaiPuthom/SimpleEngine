#include "stdafx.h"
#include "Engine/global.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Engine/Graphics/Texture/Texture.h"

Mesh::Mesh()
	: myShader(std::make_shared<Shader>())
	, myFrameBuffer(std::make_unique<ConstantBuffer>())
	, myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myTimeBuffer(std::make_unique<ConstantBuffer>())
	, myDirectionLightBuffer(std::make_unique<ConstantBuffer>())
	, myTexture(std::make_unique<Texture>())
	, myTexture2(std::make_unique<Texture>())
	, myGraphicsEngine(nullptr)
{
}

Mesh::~Mesh()
{
}

const bool Mesh::Init(const MeshData& aMeshData, const char* aPSShaderFile, const char* aVSShaderFile, const char* aTextureFile)
{
	myGraphicsEngine = SimplyGlobal::GetGraphicsEngine();

	myMeshData = aMeshData;
	myModelToWorld = SimpleUtilities::Matrix4x4f();

	std::shared_ptr<Camera> camera = myGraphicsEngine->GetCamera();
	auto device = myGraphicsEngine->GetDevice();

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

		HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
			return false;
	}

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

		if (!myTimeBuffer->Init(myGraphicsEngine, sizeof(TimeBufferData), &timeBuffer))
			return false;
	}

	{
		DirectionalLightBufferData directionLightBuffer =
		{
			SimpleUtilities::Vector3f(0,-1,0)
		};

		if (!myDirectionLightBuffer->Init(myGraphicsEngine, sizeof(DirectionalLightBufferData), &directionLightBuffer))
			return false;
	}

	if (!myShader->Init(device, aPSShaderFile, aVSShaderFile))
		return false;

	const bool success = myTexture->Init(device, aTextureFile);
	if (success == false)
		return false;

	const bool success2 = myTexture2->Init(device, "Assets/Uppgift6/Grass_c.dds");
	if (success2 == false)
		return false;


	myTexture->Bind(myGraphicsEngine->GetContext(), 0);
	myTexture2->Bind(myGraphicsEngine->GetContext(), 1);

	return true;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	auto context = myGraphicsEngine->GetContext();
	auto camera = myGraphicsEngine->GetCamera();

	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	myShader->SetShader(context.Get());

	FrameBufferData frameBuffer = {};
	frameBuffer.worldToClipMatrix = camera->GetWorldToClipMatrix();
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

	{ //Day & Night Cycle
		DirectionalLightBufferData directionLightBuffer = {};
		/*const float cycleDuration = 2.0f;
		const float angularVelocity = 2 * 3.14f / cycleDuration;
		const float elevationAngle = 0.5f * sin(angularVelocity * static_cast<float>(SimplyGlobal::GetTotalTime()) + cycleDuration);

		directionLightBuffer.dir.x = cos(elevationAngle);
		directionLightBuffer.dir.y = sin(elevationAngle);
		directionLightBuffer.dir.z = 0;*/

		directionLightBuffer.dir.x = 0;
		directionLightBuffer.dir.y = -1;
		directionLightBuffer.dir.z = 0; 
		directionLightBuffer.dir.Normalize();

		myDirectionLightBuffer->Bind(3);
		myDirectionLightBuffer->Update(sizeof(DirectionalLightBufferData), &directionLightBuffer);
	}


	context->DrawIndexed(static_cast<UINT>(myMeshData.myIndices.size()), 0, 0);
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
