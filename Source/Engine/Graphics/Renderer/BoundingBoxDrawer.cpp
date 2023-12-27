#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

BoundingBoxDrawer::BoundingBoxDrawer()
	: myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myColor(0,1,0,1)
{
	myShader = SimpleGlobal::GetGraphicsEngine()->GetShader("Shaders/LinePS.cso", "Shaders/DefaultVS.cso");

	myVertices.reserve(24);
	myVertices.resize(24, Vertex{});

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
		assert(false && "failed to create VertexBuffer");

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
		assert(false && "failed to create IndexBuffer");

	ObjectBufferData objectBuffer;

	if (!myObjectBuffer->Init(SimpleGlobal::GetGraphicsEngine(), sizeof(ObjectBufferData), &objectBuffer))
		assert(false && "failed to create ObjectBuffer");

	myObjectBuffer->SetSlot(1);
}

BoundingBoxDrawer::~BoundingBoxDrawer()
{
}

void BoundingBoxDrawer::Render(const std::shared_ptr<const ModelInstance> aModelInstance)
{
	const SimpleUtilities::Vector3f minPoint = aModelInstance->myMesh->myBoundingBox.min;
	const SimpleUtilities::Vector3f  maxPoint = aModelInstance->myMesh->myBoundingBox.max;

	Vertex down_SouthWest;
	down_SouthWest.position = { minPoint.x, minPoint.y, minPoint.z, 1 };
	Vertex down_SouthEast;
	down_SouthEast.position = { maxPoint.x, minPoint.y, minPoint.z, 1 };
	Vertex up_SouthWest;
	up_SouthWest.position = { minPoint.x, maxPoint.y, minPoint.z, 1 };
	Vertex up_SouthEast;
	up_SouthEast.position = { maxPoint.x, maxPoint.y, minPoint.z, 1 };
	Vertex down_NorthWest;
	down_NorthWest.position = { minPoint.x, minPoint.y, maxPoint.z, 1 };
	Vertex down_NorthEast;
	down_NorthEast.position = { maxPoint.x, minPoint.y, maxPoint.z, 1 };
	Vertex up_NorthWest;
	up_NorthWest.position = { minPoint.x, maxPoint.y, maxPoint.z, 1 };
	Vertex up_NorthEast;
	up_NorthEast.position = { maxPoint.x, maxPoint.y, maxPoint.z, 1 };

	myVertices[0] = down_SouthWest;
	myVertices[1] = down_SouthEast;
	myVertices[2] = down_SouthEast;
	myVertices[3] = up_SouthEast;
	myVertices[4] = up_SouthEast;
	myVertices[5] = up_SouthWest;
	myVertices[6] = up_SouthWest;
	myVertices[7] = down_SouthWest;

	myVertices[8] = down_NorthWest;
	myVertices[9] = down_NorthEast;
	myVertices[10] = down_NorthEast;
	myVertices[11] = up_NorthEast;
	myVertices[12] = up_NorthEast;
	myVertices[13] = up_NorthWest;
	myVertices[14] = up_NorthWest;
	myVertices[15] = down_NorthWest;

	myVertices[16] = down_SouthWest;
	myVertices[17] = down_NorthWest;
	myVertices[18] = down_SouthEast;
	myVertices[19] = down_NorthEast;
	myVertices[20] = up_SouthWest;
	myVertices[21] = up_NorthWest;
	myVertices[22] = up_SouthEast;
	myVertices[23] = up_NorthEast;

	for (auto& vertice : myVertices)
	{
		vertice.color = myColor;
	}

	auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();
	context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myVertices.data(),0, 0);

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = aModelInstance->GetMatrix();

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	myShader->SetShader(context.Get());

	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->DrawIndexed(static_cast<UINT>(myVertices.size()), 0, 0);
}

void BoundingBoxDrawer::SetLineColor(const SimpleUtilities::Vector4f& aColor)
{
	myColor = aColor;
}
