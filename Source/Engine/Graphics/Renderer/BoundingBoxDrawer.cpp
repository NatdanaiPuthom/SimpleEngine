#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"

BoundingBoxDrawer::BoundingBoxDrawer()
	: myObjectBuffer(std::make_unique<ConstantBuffer>())
{
	myShader = SimpleGlobal::GetGraphicsEngine()->GetShader("Shaders/LinePS.cso", "Shaders/DefaultVS.cso");
}

BoundingBoxDrawer::~BoundingBoxDrawer()
{
}

bool BoundingBoxDrawer::Init(std::shared_ptr<ModelInstance> aModelInstance)
{
	SimpleUtilities::Vector3f minPoint = aModelInstance->myMesh->myBoundingBox.min;
	SimpleUtilities::Vector3f  maxPoint = aModelInstance->myMesh->myBoundingBox.max;
	myMatrix = aModelInstance->GetMatrix();

	Vertex vertex1;
	vertex1.position = { minPoint.x, minPoint.y, minPoint.z, 1 };
	Vertex vertex2;
	vertex2.position = { maxPoint.x, minPoint.y, minPoint.z, 1 };
	Vertex vertex3;
	vertex3.position = { minPoint.x, maxPoint.y, minPoint.z, 1 };
	Vertex vertex4;
	vertex4.position = { maxPoint.x, maxPoint.y, minPoint.z, 1 };
	Vertex vertex5;
	vertex5.position = { minPoint.x, minPoint.y, maxPoint.z, 1 };
	Vertex vertex6;
	vertex6.position = { maxPoint.x, minPoint.y, maxPoint.z, 1 };
	Vertex vertex7;
	vertex7.position = { minPoint.x, maxPoint.y, maxPoint.z, 1 };
	Vertex vertex8;
	vertex8.position = { maxPoint.x, maxPoint.y, maxPoint.z, 1 };

	myVertices.push_back(vertex1);
	myVertices.push_back(vertex2);
	myVertices.push_back(vertex2);
	myVertices.push_back(vertex4);
	myVertices.push_back(vertex4);
	myVertices.push_back(vertex3);
	myVertices.push_back(vertex3);
	myVertices.push_back(vertex1);

	myVertices.push_back(vertex5);
	myVertices.push_back(vertex6);
	myVertices.push_back(vertex6);
	myVertices.push_back(vertex8);
	myVertices.push_back(vertex8);
	myVertices.push_back(vertex7);
	myVertices.push_back(vertex7);
	myVertices.push_back(vertex5);

	myVertices.push_back(vertex1);
	myVertices.push_back(vertex5);
	myVertices.push_back(vertex2);
	myVertices.push_back(vertex6);
	myVertices.push_back(vertex3);
	myVertices.push_back(vertex7);
	myVertices.push_back(vertex4);
	myVertices.push_back(vertex8);

	for (unsigned int i = 0; i < myVertices.size(); ++i)
	{
		myIndices.push_back(i);
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myVertices.size());
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = &myVertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result = SimpleGlobal::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(result))
		return false;

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

	result = SimpleGlobal::GetGraphicsEngine()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result))
		return false;

	ObjectBufferData objectBuffer;
	if (!myObjectBuffer->Init(SimpleGlobal::GetGraphicsEngine(), sizeof(ObjectBufferData), &objectBuffer))
		return false;

	myObjectBuffer->SetSlot(1);

	return true;
}

void BoundingBoxDrawer::Render()
{
	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = myMatrix;

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

	myShader->SetShader(context.Get());

	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->DrawIndexed(static_cast<UINT>(myVertices.size()), 0, 0);
}
